#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"


// ! Deprecated (ray_color, refactored)
/*
// return the ray_color
// * (now always black)
color ray_color(const ray& r, const hittable& world) {
    // ! Deprecated (always return color) 
    // return color(0, 0, 0.5);
    // ! Deprecated end

    // ! Deprecated (only return when the ray hits the sphere)
    // if (hit_sphere(point3(0, 0, -1), 0.5, r)) {
    //     return color(0, 0, 1);
    // }  
    // ! Deprecated end

    hit_record rec;
    if (world.hit(r, interval(0, infinity), rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1)); // * normal to color
    }

    // ! Deprecated
    // * if hit, return the color of the sphere
    // get t's value when the ray hits the sphere
    // auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    // if (t > 0.0) {
        // get sphere surface normal(hitpiont - center)
    //     vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        // return the color of the sphere (scale the normal's xyz to color rgb)
    //     return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
    // }
    // ! Deprecated end

    // * if not hit, return the background color
    vec3 unit_direction = unit_vector(r.direction());
    // scale the y component to [0, 1], unit_direction is in the range of [-1, 1], a is in the range of [0, 1]
    auto a = 0.5 * (unit_direction.y() + 1.0); 
    // blendedvalue (or, liner interpolation) of white and blue
    // * (1 - a) * white + a * blue
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}
*/
// ! Deprecated end


int main() {

    // ! Deprecated (refactored)
    /*
    // * Image

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 2400;

    // calculate the height of the image, ensure that it is at least 1.
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;


    // * World
    hittable_list world;
    // add the sphere to the world
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);
    auto camera_center = point3(0, 0, 0);

    // calculate the horizontal and vertical vectors along viewport
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0); // * notice the negative sign

    // calculate the horizontal and vertical delta vectors from pixel to pixel
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - (viewport_u / 2) - (viewport_v / 2);
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v); // * the first pixel 00's location

    // ! Deprecated
    // int image_width = 256;
    // int image_height = 256;
    // ! Deprecated end

    // * Render
    // Output but store in a ppm format later
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = 0; j < image_height; j ++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << "" << std::flush;
        for (int i = 0; i < image_width; i ++) {
            // ! Deprecated (ppm test)
            // auto r = double(i) / (image_width - 1);
            // auto g = double(j) / (image_height - 1);
            // auto b = 1.0;
            // scale them to 0-255
            // int ir = int(255.999 * r);
            // int ig = int(255.999 * g);
            // int ib = int(255.999 * b);
            // std::cout << ir << " " << ig << " " << ib << "\n";
            // ! Deprecated end

            // ! Deprecated (color test)
            // auto pixel_color = color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0);
            // ! Deprecated end
            
            auto pixel_center = pixel00_loc +(i * pixel_delta_u) + (j * pixel_delta_v); // * the center of the each pixel
            auto ray_direction = pixel_center - camera_center; // * the direction of the ray through this pixel
            ray r(camera_center, ray_direction); // ray direction is not unit vector (easier)

            color pixel_color = ray_color(r, world); // * the color of the pixel
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\nDone.                 \n";
    */
   // ! Deprecated end

    // * new, using the new camera class
    hittable_list world;

    // add objects to the world
    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    camera cam;
    
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.render(world);
}