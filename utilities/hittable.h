#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "aabb.h"

class material;

// Stores record of an intersection point
struct hit_record{
    point3 p;
    vec3 normal;
    double t;
    shared_ptr<material> mat_ptr;

    // These are coordinates in uv texture space
    double u;
    double v;

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
        virtual bool bounding_box(double time0, double time1, aabb& output_bounding_box) const = 0;

};

// Handles the translation of objects
class translate : public hittable{
    public:
        translate(shared_ptr<hittable> hp, const vec3& displacement)
            : h_ptr(hp), offset(displacement){}
        
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_bounding_box) const override;

    public:
        shared_ptr<hittable> h_ptr;
        vec3 offset;
};

bool translate :: hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    ray r_new(r.origin()-offset, r.direction(), r.time());
    if (!h_ptr->hit(r_new, t_min, t_max, rec))
        return false;
    
    rec.p += offset;
    rec.set_face_normal(r_new, rec.normal);

    return true;
}

bool translate :: bounding_box(double time0, double time1, aabb& output_bounding_box) const {
    if (!h_ptr->bounding_box(time0, time1, output_bounding_box))
        return false;

    output_bounding_box = aabb(
        output_bounding_box.min() + offset,
        output_bounding_box.max() + offset
    );

    return true;
}

// Handles the rotation around y-axis
class rotate_y : public hittable{
    public:
        rotate_y(shared_ptr<hittable> p, double angle);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool bounding_box(double time0, double time1, aabb& output_bounding_box) const override{
            output_bounding_box = bbox;
            return true;
        }

    public:
        shared_ptr<hittable> h_ptr;
        double cos_theta;
        double sin_theta;
        bool hasBox;
        aabb bbox;
};

rotate_y :: rotate_y(shared_ptr<hittable> p, double angle) : h_ptr(p){
    auto radians = degrees_to_radians(angle);
    cos_theta = cos(radians);
    sin_theta = sin(radians);

    hasBox = h_ptr->bounding_box(0, 1, bbox);

    auto min = vec3(-infinity, -infinity, -infinity);
    auto max = vec3(infinity, infinity, infinity);

    for (int i=0;i<2;i++){
        for (int j=0;j<2;j++){
            for (int k=0;k<2;k++){
                auto x = i*bbox.min().x()+(1-i)*bbox.max().x();
                auto y = j*bbox.min().y()+(1-j)*bbox.max().y();
                auto z = k*bbox.min().z()+(1-k)*bbox.max().z();

                auto newx = cos_theta*x+sin_theta*z;
                auto newz = -sin_theta*x+cos_theta*z;

                vec3 temp(newx, y, newz);

                for (int c=0;c<3;c++){
                    min[c] = fmin(min[c], temp[c]);
                    max[c] = fmax(max[c], temp[c]);
                }
            }
        }
    }

    bbox = aabb(min, max);
}

bool rotate_y :: hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    auto origin = r.origin();
    auto direction = r.direction();

    origin[0] = cos_theta*origin[0] - sin_theta*origin[2];
    origin[2] = sin_theta*origin[0] + cos_theta*origin[2];

    direction[0] = cos_theta*direction[0] - sin_theta*direction[2];
    direction[2] = sin_theta*direction[0] + cos_theta*direction[2];

    ray rotated_r(origin, direction, r.time());
    if (!h_ptr->hit(rotated_r, t_min, t_max, rec))
        return false;
    
    auto p = rec.p;
    auto normal = rec.normal;

    p[0] = cos_theta*p[0] + sin_theta*p[2];
    p[2] = -sin_theta*p[0] + cos_theta*p[2];

    normal[0] = cos_theta*normal[0] + sin_theta*normal[2];
    normal[2] = -sin_theta*normal[0] + cos_theta*normal[2];

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}

#endif