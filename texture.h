#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"
#include "rtw_stb_image.h"

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

class image_texture : public texture { // * image texture from image file
    public:
        // take image file path and load the image
        image_texture(const char* filename) : image(filename) {}


        // value method, return the color value of the image texture based on the uv coordinates and hit point p
        color value(double u, double v, const point3& p) const override {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (image.height() <= 0) return color(0,1,1);

            // Clamp input texture coordinates to [0,1] x [1,0]
            u = interval(0,1).clamp(u);
            v = 1.0 - interval(0,1).clamp(v);  // Flip V to image coordinates

            // caluclate the pixel coordinate based on the uv coordinates
            auto i = int(u * image.width());
            auto j = int(v * image.height());

            // get RGB values of the pixel
            auto pixel = image.pixel_data(i,j);

            // scale the RGB values to [0,1]
            auto color_scale = 1.0 / 255.0;
            return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
        }

    private:
        rtw_image image; // use rtw_image to store the image data
};

#endif