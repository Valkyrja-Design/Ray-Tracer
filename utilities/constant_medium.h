#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "general.h"

#include "material.h"
#include "hittable.h"
#include "texture.h"

class constant_medium : public hittable {
    public:
        constant_medium(
            shared_ptr<hittable> b, double d, shared_ptr<texture> t
        ) : boundary(b), phase_function(make_shared<isotropic>(t)), neg_inv_density(-1/d){}

        constant_medium(
            shared_ptr<hittable> b, double d, color c
        ) : boundary(b), phase_function(make_shared<isotropic>(c)), neg_inv_density(-1/d){}

        virtual bool hit(
            const ray& r, double t_min, double t_max, hit_record& rec
        ) const override;

        virtual bool bounding_box(double time0, double time1, aabb& output_bounding_box) const override{
            boundary->bounding_box(time0, time1, output_bounding_box);
            return true;
        }
    public:
        shared_ptr<hittable> boundary;
        shared_ptr<material> phase_function;
        double neg_inv_density;
};

bool constant_medium :: hit(const ray& r, double t_min, double t_max, hit_record& rec) const{
    
    const bool enableDebug = false;
    const bool debugging = enableDebug && random_double() < 0.0001;

    // Finding t_min & t_max of hit points
    hit_record rec1, rec2;

    if (!boundary->hit(r, -infinity, infinity, rec1))
        return false;

    if (!boundary->hit(r, rec1.t+0.0001, infinity, rec2))
        return false;

    if (debugging) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

    if (rec1.t < t_min) rec1.t = t_min;
    if (rec2.t > t_max) rec2.t = t_max;

    if (rec1.t >= rec2.t) return false;

    if (rec1.t < 0) rec1.t = 0;

    const auto ray_length = r.direction().length();
    const auto distance_inside_boundary = (rec2.t - rec1.t)*ray_length;
    const auto hit_distance = neg_inv_density * log(random_double());

    if (hit_distance > distance_inside_boundary)
        return false;

    rec.t = rec1.t + hit_distance/ray_length;
    rec.p = r.at(rec.t);

    if (debugging) {
        std::cerr << "hit_distance = " <<  hit_distance << '\n'
                    << "rec.t = " <<  rec.t << '\n'
                    << "rec.p = " <<  rec.p << '\n';
    }

    rec.normal = vec3(1,0,0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.mat_ptr = phase_function;

    return true;
}

#endif