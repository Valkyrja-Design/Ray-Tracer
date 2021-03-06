#ifndef MATERIAL_H
#define MATERIAL_H

#include "general.h"
#include "hittable.h"
#include "texture.h"

struct hit_record;

class material{

    public:
        
        virtual color emitted(double u, double v, const point3& p) const {
            return color(0, 0, 0);
        }

        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material{

    public:
        lambertian(const color& a) : albedo(make_shared<solid_color>(a)){};
        lambertian(shared_ptr<texture> a) : albedo(a){}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
         ) const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            // Degenerate scatter direction 
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;
    
            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }
    
    public:
        shared_ptr<texture> albedo;
};

class metal : public material{

    public:
        metal(const color& a, double f) : albedo(a), fuzz( f < 1 ? f : 1){};

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
         ) const override {
            
            auto scatter_direction = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, scatter_direction + fuzz*random_in_unit_sphere(), r_in.time());
            attenuation = albedo;
            return dot(scattered.direction(), rec.normal) > 0;
        }

    public:
        color albedo;
        double fuzz;
};

class dielectric : public material {

    public:
        dielectric(double refractive_index) : ir(refractive_index) {};

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attentuation, ray& scattered
        ) const override {
            attentuation = color(1.0, 1.0, 1.0);
            // If ray is from air into material or vice-versa
            double refractive_ratio = rec.front_face ? (1.0/ir) : ir;

            vec3 unit_direction = unit_vector(r_in.direction());
            auto cos_theta = fmin(1.0, dot(-unit_direction, rec.normal));
            auto sin_theta = sqrt(1.0-cos_theta*cos_theta);

            auto cannot_refract = refractive_ratio * sin_theta > 1.0;
            vec3 direction;
            if (cannot_refract || reflectance(cos_theta, refractive_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refractive_ratio);
            
            scattered = ray(rec.p, direction, r_in.time());
            return true;
        }

    public:
        double ir; // Refractive index

    private:
        static double reflectance(double cosine, double ref_idx){
            // Schlick's approx. for reflectance 
            auto r0 = (1.0-ref_idx)/(1.0+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1-cosine), 5);
        }
};

// Light emitting material
class diffuse_light : public material {
    public:
        diffuse_light(shared_ptr<texture> t) : emit(t){}
        diffuse_light(color c) : emit(make_shared<solid_color>(c)){}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            // auto scatter_direction = rec.normal + random_unit_vector();

            // // Degenerate scatter direction 
            // if (scatter_direction.near_zero())
            //     scatter_direction = rec.normal;
    
            // scattered = ray(rec.p, scatter_direction, r_in.time());
            // attenuation = emit->value(rec.u, rec.v, rec.p);
            // return true;
            return false;
        }

        virtual color emitted(double u, double v, const point3& p) const override {
            return emit->value(u, v, p);
        }

    public:
        shared_ptr<texture> emit;
};

class isotropic : public material {
    public:
        isotropic(color c) : albedo(make_shared<solid_color>(c)){}
        isotropic(shared_ptr<texture> t) : albedo(t) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        shared_ptr<texture> albedo;
};

#endif