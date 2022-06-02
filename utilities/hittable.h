#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

// Stores record of an intersection point
struct hit_record{
    point3 p;
    vec3 normal;
    double t;
    // normal always points outward

    bool front_face;
    inline void set_face_normal(const ray&r, const vec3& outward_normal){
        front_face = (dot(r.direction(), outward_normal) < 0);
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract class for all hittable objects 
class hittable{

    public:
        virtual bool hit(const ray&r, double t_min, double t_max, hit_record& rec) const = 0;

};
#endif