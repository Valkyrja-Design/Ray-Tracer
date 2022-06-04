#ifndef CAMERA_H
#define CAMERA_H

#include "general.h"

class camera{

    public:
        camera(){
            auto aspect_ratio = 16.0/9.0;
            auto viewport_height = 2.0;
            auto viewport_width = aspect_ratio*viewport_height;
            auto focal_length = 1.0;

            origin = point3(0, 0, 0);
            horizontal = point3(viewport_width, 0, 0);
            vertical = point3(0, viewport_height, 0);
            lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);
        }

        inline ray get_ray(double u, double v) const {
            return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        }

    public:
        point3 origin;
        vec3 horizontal;
        vec3 vertical;
        point3 lower_left_corner;
};

#endif