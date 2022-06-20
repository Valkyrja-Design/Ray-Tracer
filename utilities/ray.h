#ifndef RAY_H
#define RAY_H

#include "vec3.h"

struct RayResult{
    int index;
    color col;
};

class ray{
    
    public:
        ray(){}
        ray(const point3& origin, const vec3& direction, double time = 0.0)
            : orig(origin), dir(direction), tm(time) {}

        vec3 direction() const { return dir; }
        point3 origin() const { return orig; }
        double time() const { return tm; }
        
        point3 at(double t) const {
            return orig+t*dir;
        }

    public:
        point3 orig;
        vec3 dir;
        double tm;
};

#endif