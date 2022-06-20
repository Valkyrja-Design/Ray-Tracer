#ifndef AABB_H
#define AABB_H

#include "general.h"

class aabb {

    public:
        aabb (){}
        aabb (const point3 min, const point3 max) : minimum(min), maximum(max) {

        }

        point3 minimum, maximum;
};

#endif