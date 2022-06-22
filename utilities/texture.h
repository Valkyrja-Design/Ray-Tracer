#ifndef TEXTURE_H
#define TEXTURE_H

#include "general.h"

#include "perlin.h"

// Abstract texture class
class texture {
    public:
        virtual color value(double u, double v, const point3& p) const = 0;
};

// Constant color texture
class solid_color : public texture {

    public:
        solid_color(){}
        solid_color(color c) : color_value(c) {}
        solid_color(double r, double g, double b) : solid_color(color(r, g, b)) {}

        virtual color value(double u, double v, const point3& p) const override {
            return color_value;
        }

    public:
        color color_value;
};

// Checker texture
class checker_texture : public texture{
    public:
        checker_texture(){}
        checker_texture(
            shared_ptr<texture> _even, shared_ptr<texture> _odd, int _scale = 10
        ) : even(_even), odd(_odd), scale(_scale) {}

        checker_texture(color c1, color c2, int _scale = 10)
            : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)), scale(_scale) {}
        
        virtual color value(double u, double v, const point3& p) const override {
            auto sines = sin(scale*p.x())*sin(scale*p.y())*sin(scale*p.z());
            if (sines < 0)
                return odd->value(u, v, p);
            else    
                return even->value(u, v, p);
        }

    public:
        shared_ptr<texture> even;
        shared_ptr<texture> odd;
        int scale;
};

class noise_texture : public texture {
    public:
        noise_texture(){}
        noise_texture(double sc) : scale(sc) {}

        virtual color value(double u, double v, const point3& p) const override{
            // return color(1.0, 1.0, 1.0) * (noise.turbulance(scale * p));
            // Marble-like texture
            return color(1,1,1) * 0.5 * (1 + sin(scale*p.z() + 10*noise.turbulance(p)));
        }

    public:
        perlin noise;
        double scale;
};

#endif