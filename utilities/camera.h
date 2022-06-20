#ifndef CAMERA_H
#define CAMERA_H

#include "general.h"

class camera{

    public:
        camera(
            point3 lookfrom,
            point3 lookat,
            vec3 vup,       // Up direction of the camera
            double vfov,    // Vertical field of view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist,
            double _time0 = 0,  
            double _time1 = 0
        ){
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio*viewport_height;

            w = unit_vector(lookfrom-lookat);
            // This is out of the plane of vup, w & v. Horizontal direction along the of view
            u = unit_vector(cross(vup, w));
            // This is along the plane in vertical direction
            v = unit_vector(cross(w, u));

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist * w;
            
            lens_radius = aperture / 2;
            time0 = _time0;
            time1 = _time1;
        }

        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u*rd.x()+v*rd.y();
            return ray(
                    origin + offset,
                    lower_left_corner + s*horizontal + t*vertical - origin - offset,
                    random_double(time0, time1)
            );
        }

    public:
        point3 origin;
        vec3 horizontal;
        vec3 vertical;
        point3 lower_left_corner;
        vec3 u, v, w;
        double lens_radius;
        double time0;   // Shutter open and close times 
        double time1;
};

#endif