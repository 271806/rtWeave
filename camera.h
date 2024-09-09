#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "hittable.h"
#include "pdf.h"
#include "material.h"
#include "vec3.h"
#include "hdr_texture.h"

#include <omp.h> // Include OpenMP header for multithreading support

// camera class
class camera {
    public:
        // hdr texture
        std::shared_ptr<hdr_texture> background_texture;  // 指向 HDR 纹理的指针

        // * Image
        double aspect_ratio = 1.0; // Ratio of image width over height
        int image_width = 100; // Rendered image width in pixel count
        // * for anti-aliasing
        int samples_per_pixel = 10; // Count of random samples for each pixel
        // * for recursive ray tracing
        int max_depth = 10; // Maximum depth of recursive ray tracing

        color background; // Scene background color (if no hdr texture is provided)
        
        double vfov = 90; // Vertical field-of-view in degrees

        point3 lookfrom = point3(0, 0, 0); // Point camera is looking from
        point3 lookat = point3(0, 0, -1); // Point camera is looking at
        vec3 vup = vec3(0, 1, 0); // Up vector (Camera-relative "up" direction)

        double defocus_angle = 0.0; // Variation angle of rays through each pixel
        double focus_dist = 10; // Distance from camera lookfrom point to plane of perfect focus


        void render_mt(const hittable& world, const hittable& lights) {
            initialize();

            // Initialize a 2D array to store the pixel colors
            auto image = new color*[image_height];
            for (int i = 0; i < image_height; ++i)
                image[i] = new color[image_width];

            // std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            // *create a shared variable to track progress
            int global_done_scanlines = 0;


            // Parallel rendering using OpenMP
            // ! Adjust the number of threads to suit your system
            #pragma omp parallel num_threads(18)
            {
                // Set different random seed for each thread
                srand(int(time(NULL)) ^ omp_get_thread_num());

                #pragma omp for schedule(static, image_height/(80)) // Use dynamic scheduling for load balancing
                for (int j = 0; j < image_height; j++) {
                    #pragma omp critical
                    {
                        if ((image_height - global_done_scanlines) % 10 == 0) {
                            std::clog << "\rScanlines remaining: " << (image_height - global_done_scanlines) << ' ' << std::flush;
                        }
                    }

                    #pragma omp atomic
                    ++global_done_scanlines;

                    for (int i = 0; i < image_width; i++) {
                        color pixel_color(0, 0, 0); 

                        for (int s_j = 0; s_j < sqrt_spp; s_j++) {
                            for (int s_i = 0; s_i < sqrt_spp; s_i++) {
                                ray r = get_ray(i, j, s_i, s_j);
                                pixel_color += ray_color(r, max_depth, world, lights);
                            }

                            image[j][i] = pixel_color * pixel_samples_scale;
                        }
                    }
                }
            }
            

            // Output the stored pixel colors
            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
            for (int j = 0; j < image_height; ++j) {
                for (int i = 0; i < image_width; ++i) {
                    write_color(std::cout, image[j][i]);
                }
            }

            // Clean up memory
            for (int i = 0; i < image_height; ++i)
                delete[] image[i];
            delete[] image;

            std::clog << "\nDone.                 \n";
        }


        void render(const hittable& world, const hittable& lights) {
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
                            pixel_color += ray_color(r, max_depth, world, lights); // calculate & accumulate the color
                            }
                        }
                    
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

        color ray_color(const ray& r, int depth, const hittable& world, const hittable& lights
        ) const {
            // * If we've exceeded the ray bounce limit, no more light is gathered.
            if (depth <= 0)
                return color(0, 0, 0);

            hit_record rec;

            // If the ray doesn't hit anything, return the background color.
            if (!world.hit(r, interval(0.001, infinity), rec)){
                if (background_texture) {
                    vec3 unit_direction = unit_vector(r.direction());
                    double u = 0.5 + atan2(unit_direction.z(), unit_direction.x()) / (2 * pi);
                    double v = 0.5 - asin(unit_direction.y()) / pi;
                    return background_texture->value(u, v);  // 使用 HDR 环境贴图
                } else {
                    return background; // 使用背景颜色
                }
            }

            scatter_record srec;
            // ray scattered;
            // color attenuation;
            // double pdf_value;
            // Calculate the color emitted by the material at the hit point.
            color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

            // If the material doesn't scatter the ray, return the emitted color.
            if (!rec.mat->scatter(r, rec, srec))
                return color_from_emission;

            if (srec.skip_pdf) {
                return srec.attenuation * ray_color(srec.skip_pdf_ray, depth-1, world, lights);
            }

            auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
            mixture_pdf p(light_ptr, srec.pdf_ptr);

            ray scattered = ray(rec.p, p.generate(), r.time());
            auto pdf_value = p.value(scattered.direction());

            double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

            color sample_color = ray_color(scattered, depth-1, world, lights);

            // * Compute the color from the scattered ray, multiply by the attenuation and the PDF
            // * The result is divided by the PDF to correctly normalize the color contribution
            color color_from_scatter = 
                (srec.attenuation * scattering_pdf * sample_color) / pdf_value;

            // Return the sum of the emitted color and the scattered color.
            return color_from_emission + color_from_scatter;
    }
};

#endif