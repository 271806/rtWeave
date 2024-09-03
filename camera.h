#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"
#include "vec3.h"

// camera class
class camera {
    public:
        // * Image
        double aspect_ratio = 1.0; // Ratio of image width over height
        int image_width = 100; // Rendered image width in pixel count
        // * for anti-aliasing
        int samples_per_pixel = 10; // Count of random samples for each pixel
        // * for recursive ray tracing
        int max_depth = 10; // Maximum depth of recursive ray tracing

        color background; // Scene background color
        
        double vfov = 90; // Vertical field-of-view in degrees

        point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
        point3 lookat = point3(0, 0, -1); // Point camera is looking at
        vec3 vup = vec3(0, 1, 0); // Up vector (Camera-relative "up" direction)

        double defocus_angle = 0.0; // Variation angle of rays through each pixel
        double focus_dist = 10; // Distance from camera lookfrom point to plane of perfect focus


        void render(const hittable& world) {
            initialize();

            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int j = 0; j < image_height; j++) {
                // print the progress
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; i++) {
                    color pixel_color(0, 0, 0); // initialize the pixel color as black

                    // * stratified sampling
                    for (int s_j = 0; s_j < sqrt_spp; s_j++) { // stratified sampling in y direction
                        for (int s_i = 0; s_i < sqrt_spp; s_i++) { // stratified sampling in x direction
                            // generate a ray through the pixel with random offset with stratified sampling
                            ray r = get_ray(i, j, s_i, s_j);
                            pixel_color += ray_color(r, max_depth, world); // calculate & accumulate the color
                            }
                        }
                    
                    // * anti-aliasing, mutiple samples per pixel, accumulate the color
                    // for (int sample = 0; sample < samples_per_pixel; sample++) {
                    //     ray r = get_ray(i, j); // generate a ray through the pixel with random offset
                    //     pixel_color += ray_color(r, max_depth, world); // calculate & accumulate the color
                    // }


                    // ! Deprecated (without anti-aliasing)
                    /*
                    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v); // * the center of the each pixel
                    auto ray_direction = pixel_center - center; // * the direction of the ray through this pixel
                    ray r(center, ray_direction); // ray direction is not unit vector (easier)

                    color pixel_color = ray_color(r, world); // * the color of the pixel
                    write_color(std::cout, pixel_color);
                    */
                   // ! Deprecated end
                    
                    // scale the accumulated color and output to the output stream
                    write_color(std::cout, pixel_samples_scale * pixel_color);
                }
            }

            std::clog << "\nDone.                 \n";
        }


    private:
        int    image_height;   // Rendered image height
        double pixel_samples_scale; // Color scale factor for a sum of pixel samples

        int sqrt_spp; // Square root of number of samples per pixel
        double recip_sqrt_spp; // 1 / sqrt_spp

        point3 center;         // Camera center
        point3 pixel00_loc;    // Location of pixel 0, 0
        vec3   pixel_delta_u;  // Offset to pixel to the right
        vec3   pixel_delta_v;  // Offset to pixel below
        vec3   u, v, w;        // Camera frame basis vectors
        vec3 defocus_disk_u;   // Defocus disk horizontal radius
        vec3 defocus_disk_v;   // Defocus disk vertical radius

        void initialize() {
            // calculate the height of the image, ensure that it is at least 1.
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            sqrt_spp = int(std::sqrt(samples_per_pixel)); // calculate the square root of samples per pixel
            // pixel_samples_scale = 1.0 / samples_per_pixel;
            pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp); // calculate the scale factor for pixel samples
            recip_sqrt_spp = 1.0 / sqrt_spp; // calculate the reciprocal of the square root of samples per pixel

            center = lookfrom; // Camera center is the lookfrom point

            // Determine viewport dimensions.
            // auto focal_length = (lookfrom - lookat).length(); // Distance from camera to lookat point
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h * focus_dist;
            auto viewport_width = viewport_height * (double(image_width)/image_height);

            // ? check figure 20 in rtoneweekend
            // Calculate the u,v,w unit basis vectors for the camera coordinate frame
            w = unit_vector(lookfrom - lookat); // Camera looks towards the lookat point
            u = unit_vector(cross(vup, w)); // u is perpendicular to w and vup
            v = cross(w, u); // v is perpendicular to w and u

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            vec3 viewport_u = viewport_width * u; // Vector across viewport horizontal edge
            vec3 viewport_v = viewport_height * (-v); // Vector down viewport vertical edge


            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate the location of the upper left pixel.
            // auto viewport_upper_left =
            //     center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
            auto viewport_upper_left = center - (focus_dist * w) - (viewport_u/2) - (viewport_v/2);
            // * the first pixel 00's location
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            // Calculate the camera defocus disk basis vectors
            auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = defocus_radius * u;
            defocus_disk_v = defocus_radius * v;
        }
        

        // * for anti-aliasing
        ray get_ray(int i, int j, int s_i, int s_j) const {
            // * with defocus feature
            // * Construct a camera ray originating from the defocus disk and directed at a randomly
            // * // sampled point around the pixel location i, j for stratified sample square s_i, s_j
            // auto offset = sample_square(); // generate a random offset for offsetting the ray for AA
            auto offset = sample_square_stratified(s_i, s_j); // generate a stratified random offset for offsetting the ray for AA
            auto pixel_sample = pixel00_loc
                                + ((i + offset.x()) * pixel_delta_u)
                                + ((j + offset.y()) * pixel_delta_v);
            // * ray_origin with defocus feature
            auto ray_origin = (defocus_angle <= 0) ? center: defocus_disk_sample(); // ray origin is the camera center, or at the defocus disk
            auto ray_direction = pixel_sample - ray_origin; // ray direction is the vector from the origin to the sampled pixel

            auto ray_time = random_double(); // * random time for motion blur
            
            return ray(ray_origin, ray_direction, ray_time); // return the sample ray
        }

        vec3 sample_square_stratified(int s_i, int s_j) const {
            // Returns the vector to a random point in the square sub-pixel specified by grid
            // indices s_i and s_j, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].

            auto px = ((s_i + random_double()) * recip_sqrt_spp) - 0.5; // calculate the x offset in the sub-pixel
            auto py = ((s_j + random_double()) * recip_sqrt_spp) - 0.5; // calculate the y offset in the sub-pixel

            return vec3(px, py, 0); // return the random offset of x and y
        }


        // ? (In addition to the new sample_square() function above, 
        // ? you'll also find the function sample_disk() in the Github source code. 
        // ? This is included in case you'd like to experiment with non-square pixels, 
        // ? but we won't be using it in this book. 
        // ? sample_disk() depends on the function random_in_unit_disk() which is defined later on.)
        vec3 sample_square() const {
            // * Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
            return vec3(random_double() - 0.5, random_double() - 0.5, 0);
        }


        point3 defocus_disk_sample() const {
            // * Returns a random point on the camera defocus disk.
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        color ray_color(const ray& r, int depth, const hittable& world) const {
            // * If we've exceeded the ray bounce limit, no more light is gathered.
            if (depth <= 0)
                return color(0, 0, 0);

            hit_record rec;

            // If the ray doesn't hit anything, return the background color.
            if (!world.hit(r, interval(0.001, infinity), rec))
                return background;

            ray scattered;
            color attenuation;
            double pdf_value;
            // Calculate the color emitted by the material at the hit point.
            color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

            // If the material doesn't scatter the ray, return the emitted color.
            if (!rec.mat->scatter(r, rec, attenuation, scattered, pdf_value))
                return color_from_emission;

            // * Light sampling
            // Randomly pick a point on the light source within specified x and z ranges
            auto on_light = point3(random_double(213,343), 554, random_double(227,332));

            // Calculate the vector from the hit point to the selected point on the light
            auto to_light = on_light - rec.p;

            // Compute the square of the distance between the hit point and the light source
            auto distance_squared = to_light.length_squared();

            // Normalize the 'to_light' vector to get a unit direction vector
            to_light = unit_vector(to_light);

            // Check if the light is facing away from the surface (dot product is negative)
            // If the light is facing away, return only the emitted color (no contribution from the light)
            if (dot(to_light, rec.normal) < 0)
                return color_from_emission;

            // Calculate the area of the light source
            double light_area = (343-213)*(332-227);

            // Calculate the cosine of the angle between the light direction and the light's normal (y-axis)
            auto light_cosine = std::fabs(to_light.y());

            // If the cosine is too small, return only the emitted color (light contribution is negligible)
            if (light_cosine < 0.000001)
                return color_from_emission;

            // Compute the PDF value for sampling the light source
            pdf_value = distance_squared / (light_cosine * light_area);

            // Create the scattered ray pointing from the hit point to the light source
            scattered = ray(rec.p, to_light, r.time());

            // Compute the scattering PDF for the scattered ray
            double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

            // * naive implementation
            // Recursively compute the color from the scattered ray, multiplied by the attenuation factor.
            // color color_from_scatter = attenuation * ray_color(scattered, depth-1, world);

            // * Calculate the scattering PDF value
            // double scattering_pdf = rec.mat -> scattering_pdf(r, rec, scattered);
            // pdf_value = scattering_pdf;
            
            // * Test: Instead of using the scattering PDF, we use a uniform PDF over the hemisphere.
            // Instead of using the scattering PDF, we use a uniform PDF over the hemisphere.
            // This means that we assume equal probability for all directions in the hemisphere.
            // double pdf_value = 1 / (2*pi);

            // * Compute the color from the scattered ray, multiply by the attenuation and the PDF
            // * The result is divided by the PDF to correctly normalize the color contribution
            color color_from_scatter = 
                (attenuation * scattering_pdf * ray_color(scattered, depth - 1, world)) / pdf_value;


            // Return the sum of the emitted color and the scattered color.
            return color_from_emission + color_from_scatter;

            // * uncomment the following code to see previous implementation (skybox as background)
            // // -------------------------------------------------------------
            // if (world.hit(r, interval(0.001, infinity), rec)) {
                
                /* temporily hide
                // return 0.5 * (rec.normal + color(1,1,1)); // * normal to color
                // * random direction on the hemisphere (most basic diffuse material model)
                // vec3 direction = random_on_hemisphere(rec.normal);
                
                // * lanbertian reflectance (more realistic diffuse material model,
                // * more light is reflected in the direction of the normal)
                // rec.normal is the center of the hemisphere,
                //  and random_unit_vector() is the random direction on the hemisphere
                vec3 direction = rec.normal + random_unit_vector();
                return 0.7 * // * reflect XX% of the light (color)
                    ray_color(ray(rec.p, direction), depth - 1, world); // * recursive ray tracing
                */
               
            //     // * scatter the ray
            //     ray scattered;
            //     color attenuation;
            //     // determine if the ray is scattered
            //     if (rec.mat->scatter(r, rec, attenuation, scattered))
            //         // if the ray is scattered, return the scattered ray's color,
            //         // multiply the attenuation
            //         // it means the ray is scattered, and the color is attenuated
            //         return attenuation * ray_color(scattered, depth - 1, world);
            //     // if the ray is not scattered, return black means no light is gathered
            //     return color(0, 0, 0);
            // }


            // // * if not hit, return the background color
            // vec3 unit_direction = unit_vector(r.direction());
            // // scale the y component to [0, 1], unit_direction is in the range of [-1, 1], a is in the range of [0, 1]
            // auto a = 0.5*(unit_direction.y() + 1.0);
            // // blendedvalue (or, liner interpolation) of white and blue
            // // * (1 - a) * white + a * blue
            // return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
            // -------------------------------------------------------------
    }
};

#endif