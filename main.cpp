#include "utilities/general.h"

#include "utilities/hittable_list.h"
#include "utilities/sphere.h"
#include "utilities/color.h"
#include "utilities/camera.h"
#include "utilities/material.h"
#include "utilities/moving_sphere.h"
#include "utilities/texture.h"

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

color ray_color(const ray& r, const hittable& world, int depth){
    hit_record rec;

    if (depth <= 0)
        return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        // 1st diffuse method (offset by normal)
        // point3 target = rec.p + rec.normal + random_unit_vector();
        
        // 2nd diffuse method (point in same hemisphere as normal (no offset)) Hemispherical scattering
        // point3 target = rec.p + random_in_hemisphere(rec.normal);
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)){
            return attenuation * ray_color(scattered, world, depth-1);
        }
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y()+1.0);
    // Blends color between white and blue according to the y height of ray direction
    return (1.0-t)*color(1.0, 1.0, 1.0)+t*color(0.5, 0.7, 1.0);
}   

int main(){
    // Image dimensions
    const auto aspect_ratio = 16.0/9.0;
    const int image_width = 800;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    const int pixelCount = image_height*image_width;

    // Image 
    vec3* image = new vec3[image_height*image_width];
    memset(&image[0], 0, image_height * image_width * sizeof(vec3));
    // cout<<(sizeof(vec3))<<"\n";

    // World

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;

    switch (0) {
        case 1:
            world = random_scene();
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            aperture = 0.1;
            break;

        default:
        case 2:
            world = two_spheres();
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;
    }

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
            [&cam, &world, &samples_per_pixel, i, j, image_width, image_height, &cvResults]() -> RayResult{
                color pixel_color(0, 0, 0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (image_width-1);
                    auto v = (j + random_double()) / (image_height-1);
                    ray r = cam.get_ray(u, v);
                    pixel_color += ray_color(r, world, max_depth);
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

    for (future<RayResult>& rr : futures){
        // cout<<"Before get\n";
        RayResult result = rr.get();
        // cout<<"After get\n";
        image[result.index] = result.col;
        
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