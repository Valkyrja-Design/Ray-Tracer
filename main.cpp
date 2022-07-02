#include "utilities/general.h"

#include "utilities/hittable_list.h"
#include "utilities/sphere.h"
#include "utilities/color.h"
#include "utilities/camera.h"
#include "utilities/material.h"
#include "utilities/moving_sphere.h"
#include "utilities/texture.h"
#include "utilities/aarect.h"
#include "utilities/box.h"
#include "utilities/constant_medium.h"
#include "utilities/bvh.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <cstring>
#include <mutex>
#include <utility>
#include <condition_variable>
#include <future>

using namespace std;

hittable_list random_scene() {
    hittable_list world;

    auto even_checker = make_shared<checker_texture>(color(0.5, 0.5, 0.5), color(0.4, 0.8, 0.9), 100);
    auto odd_checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9), 1);
    auto checker = make_shared<checker_texture>(even_checker, odd_checker);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + point3(0, random_double(0, 0.5), 0);
                    world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

hittable_list two_spheres() {
    hittable_list objects;

    // auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto even_checker = make_shared<checker_texture>(color(0.5, 0.5, 0.5), color(0.4, 0.8, 0.1), 100);
    auto odd_checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.4, 0.4, 0.8), 20);
    auto checker = make_shared<checker_texture>(even_checker, odd_checker);

    objects.add(make_shared<sphere>(point3(0,-2, 0), 2, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto perlin_texture = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(perlin_texture)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(perlin_texture)));

    return objects;
}

hittable_list image_textures() {
    hittable_list objects;
    auto _texture = make_shared<image_texture>("texture images/Renne (1).png");
    auto _surface = make_shared<lambertian>(_texture);
    auto _texture1 = make_shared<image_texture>("texture images/Beautiful Mona.jpg");
    auto _surface1 = make_shared<lambertian>(_texture1);
    auto light_source = make_shared<diffuse_light>(color(4, 4, 4));
    objects.add(make_shared<sphere>(point3(0, 0, -2), 2, _surface1));
    objects.add(make_shared<sphere>(point3(0, 0, 2), 2, _surface));
    objects.add(make_shared<sphere>(point3(0, 3, 0), 5, light_source));
    return objects;
}

hittable_list simple_light() {
    hittable_list objects;

    // auto pertext = make_shared<noise_texture>(4);
    // objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    // objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto _texture = make_shared<image_texture>("texture images/Renne (1).png");
    auto _texture1 = make_shared<image_texture>("texture images/Beautiful Mona.jpg");
    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    auto _checker = make_shared<checker_texture>(color(0, 0, 0), color(1, 1, 1));
    auto _floor = make_shared<lambertian>(_checker);
    auto _surface = make_shared<lambertian>(_texture);
    auto _surface1 = make_shared<lambertian>(_texture1);

    objects.add(make_shared<sphere>(point3(0, 6, 0), 1, difflight));
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, _floor));
    objects.add(make_shared<sphere>(point3(0, 2, -3), 2, _surface1));
    objects.add(make_shared<sphere>(point3(0, 2, 3), 2, _surface));

    objects.add(make_shared<xy_rect>(2, 5, 1, 3, 5, difflight));
    objects.add(make_shared<xy_rect>(2, 4, 1, 3, -5, difflight));
    // objects.add(make_shared<xy_rect>(1, 4, 1, 3, -1.5, difflight));
    objects.add(make_shared<xy_rect>(1, 4, 1, 3, 0.5, difflight));


    return objects;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<box>(point3(130, 0, 65), point3(295, 165, 230), white));
    objects.add(make_shared<box>(point3(265, 0, 295), point3(430, 330, 460), white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);

    return objects;
}

hittable_list cornell_smoke() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    objects.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));

    return objects;
}

hittable_list final_scene() {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0,y0,z0), point3(x1,y1,z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("texture images/Beautiful Mona.jpg"));
    objects.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    // auto pertext = make_shared<noise_texture>(0.1);
    // objects.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
            vec3(-100,270,395)
        )
    );

    return objects;
}


color ray_color(const ray& r, const color& background, const hittable& world, int depth){
    hit_record rec;

    // If max depth is reached no more light is scattered
    if (depth <= 0)
        return color(0,0,0);

    // If ray hits nothing we return the background color
    if (!world.hit(r, 0.001, infinity, rec)){
        return background;
    }

    ray scattered;
    color attenuation;
    color emmited = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emmited;

    return emmited + attenuation * ray_color(scattered, background, world, depth-1);
}   

int main(){
    // Image dimensions
    auto aspect_ratio = 16.0/9.0;
    int image_width = 800;
    const int max_depth = 50;

    int samples_per_pixel = 200;

    // cout<<(sizeof(vec3))<<"\n";

    // World

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background;

    switch (0) {
        // Random scene
        case 1:
            world = random_scene();
            lookfrom = point3(13,2,3);
            background = color(0.7, 0.8, 1.0);
            lookat = point3(0,0,0);
            vfov = 20.0;
            aperture = 0.1;
            break;

        // Scene with two spheres
        case 2:
            world = two_spheres();
            lookfrom = point3(13,2,3);
            background = color(0.7, 0.8, 1.0);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;

        // Scene with two perlin texture spheres
        case 3:
            world = two_perlin_spheres();
            lookfrom = point3(13,2,3);
            background = color(0.7, 0.8, 1.0);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;
    
        // Scene with image textures applied onto spheres
        case 4:
            world = image_textures();
            lookfrom = point3(13,2,3);
            background = color(0.7, 0.8, 1.0);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;
        
        // Scene with light sources
        case 5:
            world = simple_light();
            samples_per_pixel = 400;
            background = color(0,0,0);
            lookfrom = point3(26,3,6);
            lookat = point3(0,2,0);
            vfov = 20.0;
            break;

        case 6:
            world = cornell_box();
            aspect_ratio = 1.0;
            image_width = 600;
            samples_per_pixel = 200;
            background = color(0,0,0);
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
    
        case 7:
            world = cornell_smoke();
            aspect_ratio = 1.0;
            image_width = 600;
            samples_per_pixel = 200;
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
        
        default:
        case 8:
            world = final_scene();
            // aspect_ratio = 1.0;
            image_width = 800;
            samples_per_pixel = 200;
            background = color(0,0,0);
            lookfrom = point3(478, 278, -600);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
    }

    // Image 
    int image_height = static_cast<int>(image_width / aspect_ratio);
    const int pixelCount = image_height*image_width;
    vec3* image = new vec3[image_height*image_width];
    memset(&image[0], 0, image_height * image_width * sizeof(vec3));

    // Camera

    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    // Multithreading 

    mutex m;
    condition_variable cvResults;
    vector<future<RayResult>> futures;

    // Render 
      
    auto curr_time = std::chrono::high_resolution_clock::now();

    for (int j = image_height-1; j>=0 ; j--) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto future_ = async(launch::async | launch::deferred, 
            [&cam, &world, &samples_per_pixel, &background, i, j, image_width, image_height, &cvResults]() -> RayResult{
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (image_width-1);
                    auto v = (j + random_double()) / (image_height-1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, background, world, max_depth);
                }
                // write_color(std::cout, pixel_color, samples_per_pixel);
                pixel_color /= samples_per_pixel;
                pixel_color = color(sqrt(pixel_color[0]), sqrt(pixel_color[1]), sqrt(pixel_color[2]));
                int index = (image_height-j-1)*image_width + i;
                RayResult result;
                result.index = index;
                result.col = pixel_color;
                return result;
            });

            {
                lock_guard<mutex> lk(m);
                futures.push_back(move(future_));
                // cerr<<futures.size()<<" "<<std::flush;
            }
        }
    }
    {
        unique_lock<mutex> lk(m);
        // cerr<<"\nSize before : "<<futures.size()<<"\n";
        cvResults.wait(lk, [&futures, &pixelCount]{
            return futures.size() == pixelCount;
        });
        // cerr<<"Size after : "<<futures.size();
    }
    // std:flush(cerr);
    int cnt = futures.size();
    for (future<RayResult>& rr : futures){
        // cout<<"Before get\n";
        RayResult result = rr.get();
        // cout<<"After get\n";
        image[result.index] = result.col;
        cerr<<"\rPixels remaining: "<<(--cnt)<<" "<<std::flush;
    }

    cerr<<"\nDone.\n";
    auto time_taken = (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now()-curr_time));
    int timeMs = static_cast<int>(time_taken.count());
    cerr<<"Time taken : "<<timeMs<<" s.\n";

    string filename = "image.ppm";
    ofstream fileHandler;
    fileHandler.open(filename, ios::out | ios::binary);
    if (!fileHandler.is_open()){
        return 0;
    }

    fileHandler<<"P3\n"<<image_width<<" "<<image_height<<"\n255\n";
    for (unsigned int i=0;i<(image_height * image_width);i++){
        auto r = image[i].x();
        auto g = image[i].y();
        auto b = image[i].z();
        fileHandler <<static_cast<int>(256*clamp(r, 0, 0.999))<<" "
                    <<static_cast<int>(256*clamp(g, 0, 0.999))<<" "
                    <<static_cast<int>(256*clamp(b, 0, 0.999))<<"\n";
    }

    cerr<<"File saved.\n";
    fileHandler.close();
    delete[] image;

    return 0;
}