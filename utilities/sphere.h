#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"


class sphere : public hittable{

    public:

        sphere(){};
        sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
        
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_bounding_box) const override;

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

bool sphere :: hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    auto oc = r.origin() - center;
    auto b_half = (dot(r.direction(), oc));
    auto a = r.direction().length_squared();
    auto c = oc.length_squared()-radius*radius;
    auto discriminant = b_half*b_half-a*c;
    if (discriminant < 0){
        return false;
    } 
    
    auto sqrtd = sqrt(discriminant);

    // nearest root in the acceptable range
    auto root = (-b_half-sqrtd)/a;
    if (root < t_min || root > t_max){
        root = (-b_half+sqrtd)/a;
        if (root < t_min || root > t_max){
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    return true;
}

bool sphere :: bounding_box(double time0, double time1, aabb& output_bounding_box) const {
    output_bounding_box = aabb(
        center - point3(radius, radius, radius),
        center + point3(radius, radius, radius)
    );
    return true;
}

#endif