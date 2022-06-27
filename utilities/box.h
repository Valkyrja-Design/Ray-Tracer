#ifndef BOX_H
#define BOX_H

#include "hittable_list.h"
#include "aarect.h"

#include "general.h"

class box : public hittable {
    public:
        box(){}
        box(const point3& p0, const point3& p1, shared_ptr<material> mp);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

        virtual bool bounding_box(
            double time0, double time1, aabb& output_bounding_box
        ) const override {
            output_bounding_box = aabb(box_min, box_max);
            return true;
        }

    public:
        point3 box_min;
        point3 box_max;
        hittable_list sides;
};

box :: box(const point3& p0, const point3& p1, shared_ptr<material> mp){
    box_min = p0;
    box_max = p1;

    // Sides parallel to z-axis
    sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mp));
    sides.add(make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mp));

    // Sides parallel to y-axis
    sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mp));
    sides.add(make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mp));
    
    // Sides parallel to x-axis
    sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mp));
    sides.add(make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mp));
}
bool box :: hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    return sides.hit(r, t_min, t_max, rec);
}

#endif