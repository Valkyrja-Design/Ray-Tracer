#include "utilities/general.h"

#include "utilities/hittable_list.h"
#include "utilities/sphere.h"
#include "utilities/color.h"
#include "utilities/camera.h"
#include "utilities/material.h"

#include <iostream>

using namespace std;

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
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    hittable_list world;
    
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    // Negative radius sphere can be used to make a hollow sphere
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    // Camera 

    camera cam;

    // Render 
    
    cout<<"P3\n"<<image_width<<" "<<image_height<<"\n255\n";
    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    cerr<<"\nDone.\n";
}