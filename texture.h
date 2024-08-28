#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"

class texture {
    public:
        virtual ~texture() = default; // * virtual destructor

        // pure virtual function to get the color value
        // this method take the u, v coordinates of the texture and the hit point p to get the color value
        virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture { // define a 
    public:
        solid_color(const color& albedo) : albedo(albedo) {} // constructor, take a color value and store it in albedo


        // constructor, take three double values and store it in albedo
        solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}


        // value method, return the albedo
        color value(double u, double v, const point3& p) const override {
            return albedo; // return the color value
        }


    private:
        color albedo; // for storing the color value
};


class checker_texture : public texture {
    public:
        // constructor with scale, even texture and odd texture
        checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd)
        : inv_scale(1.0 / scale), even(even), odd(odd) {}

        // constructor with scale and two color values
        checker_texture(double scale, const color& c1, const color& c2)
        : checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}


        // value method, return the color value of checker board based on point p in 3d space
        color value(double u, double v, const point3& p) const override {
            // get the integer xyz value of the sclaed pointp
            auto xInteger = int(std::floor(inv_scale * p.x()));
            auto yInteger = int(std::floor(inv_scale * p.y()));
            auto zInteger = int(std::floor(inv_scale * p.z()));

            // check if the sum of the integer values is even
            bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

            // return the color value based on the even or odd
            return isEven ? even->value(u, v, p) : odd->value(u, v, p);
        }

    private:
        double inv_scale; // inverse scale, to control the size of the checker pattern
        shared_ptr<texture> odd; // texture for the odd part of the checker pattern
        shared_ptr<texture> even; // texture for the even part of the checker pattern
};

#endif