#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "hittable.h"

// camera class
class camera {
    public:
        // * Image
        double aspect_ratio = 1.0; // Ratio of image width over height
        int image_width = 100; // Rendered image width in pixel count


        void render(const hittable& world) {
            initialize();

            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int j = 0; j < image_height; j++) {
                // print the progress
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {
                    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v); // * the center of the each pixel
                    auto ray_direction = pixel_center - center; // * the direction of the ray through this pixel
                    ray r(center, ray_direction); // ray direction is not unit vector (easier)

                    color pixel_color = ray_color(r, world); // * the color of the pixel
                    write_color(std::cout, pixel_color);
                }
            }

            std::clog << "\nDone.                 \n";
        }


    private:
        int    image_height;   // Rendered image height
        point3 center;         // Camera center
        point3 pixel00_loc;    // Location of pixel 0, 0
        vec3   pixel_delta_u;  // Offset to pixel to the right
        vec3   pixel_delta_v;  // Offset to pixel below

        void initialize() {
            // calculate the height of the image, ensure that it is at least 1.
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            center = point3(0, 0, 0); // Camera center

            // Determine viewport dimensions.
            auto focal_length = 1.0;
            auto viewport_height = 2.0;
            auto viewport_width = viewport_height * (double(image_width)/image_height);
            
            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            auto viewport_u = vec3(viewport_width, 0, 0);
            auto viewport_v = vec3(0, -viewport_height, 0); // * notice the negative sign


            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate the location of the upper left pixel.
            auto viewport_upper_left =
                center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
            // * the first pixel 00's location
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        }

        color ray_color(const ray& r, const hittable& world) const {
            hit_record rec;

            if (world.hit(r, interval(0, infinity), rec)) {
                return 0.5 * (rec.normal + color(1,1,1)); // * normal to color
            }

            // * if not hit, return the background color
            vec3 unit_direction = unit_vector(r.direction());
            // scale the y component to [0, 1], unit_direction is in the range of [-1, 1], a is in the range of [0, 1]
            auto a = 0.5*(unit_direction.y() + 1.0);
            // blendedvalue (or, liner interpolation) of white and blue
            // * (1 - a) * white + a * blue
            return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
};

#endif