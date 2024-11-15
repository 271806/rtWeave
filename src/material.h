#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "pdf.h"
#include "hittable.h"

#include "texture.h"

// class hit_record; // will be used in the material class


class scatter_record {
  public:
    color attenuation;
    shared_ptr<pdf> pdf_ptr;
    bool skip_pdf;
    ray skip_pdf_ray;
};

class material {
    public:
        // virtual
        virtual ~material() = default;

        virtual color emitted(
            const ray& r_in, const hit_record& rec, double u, double v, const point3& p
            ) const {
            return color(0, 0, 0); // return black color by default
        }


        // * scatter the ray
        // r_in: the incoming ray
        // rec: the hit record
        // attenuation: the attenuation of the ray (attenutation means: the decrease in intensity of the light)
        // scattered: the scattered ray
        // return: whether the ray is scattered
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, scatter_record& srec
        ) const {
            return false; // return false by default, means no scattering
        }

        virtual double scattering_pdf(
            const ray& r_in, const hit_record& rec, const ray& scattered
        ) const {
            return 0; // return 0 by default
        }
};


// define the lambertian material
class lambertian : public material {
    public:
        // * constructor for texutre
        // take a color and convert it to a solid color texture
        lambertian(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}

        // take a shared_ptr of texture
        lambertian(shared_ptr<texture> tex) : tex(tex) {}

        // initialize the lambertian material with albedo
        // lambertian(const color& albedo) : albedo(albedo) {} // constructor with albedo
        
        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec)
        const override {
            srec.attenuation = tex->value(rec.u, rec.v, rec.p);
            srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
            srec.skip_pdf = false;
            return true;
            // auto scatter_direction = rec.normal + random_unit_vector();
            // ! Test: Generate a random direction on the hemisphere centered around the surface normal
            // auto scatter_direction = random_on_hemisphere(rec.normal);
            // ---

            // Create an ONB (Orthonormal Basis) using the surface normal from the hit record
            // onb uvw(rec.normal);

            // Generate a random scatter direction in the local coordinate system
            // and transform it to the world coordinate system using the ONB
            // auto scatter_direction = uvw.transform(random_cosine_direction());
            
            // Create the scattered ray with the calculated direction
            // scattered = ray(rec.p, unit_vector(scatter_direction), r_in.time());

            // if (scatter_direction.near_zero())
            //     // if the scatter direction is near zero, set it to the normal
            //     // to prevent the scatter direction to be zero
            //     scatter_direction = rec.normal;
            // scattered = ray(rec.p, scatter_direction, r_in.time());

            // attenuation = albedo;
            // * Use the texture's value method to get the color at that point as the attenuation value.
            // attenuation = tex->value(rec.u, rec.v, rec.p);

            // Calculate the PDF value for the scattered ray based on the cosine-weighted hemisphere
            // pdf = dot(uvw.w(), scattered.direction()) / pi;

            // return true;
        }

        double scattering_pdf(
            const ray& r_in, const hit_record& rec, const ray& scattered
        ) const override {
        //     auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
        //     // If the cosine of the angle between the normal and scattered direction is negative,
        //     // which is backward scattering occurs
        //     // return 0 (no scattering); otherwise, return the value of cos(theta)/pi
        //     return cos_theta < 0 ? 0 : cos_theta / pi;
            // return 1 / (2*pi); // ! Test: Uniform distribution over the hemisphere
            auto cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
            return cos_theta < 0 ? 0 : cos_theta / pi;
        }
    
    private:
        // color albedo; // albedo of the material
        shared_ptr<texture> tex; // texture of the material
};


class metal : public material {
    public:
        // take albedo and fuzziness
        // fuzziness is in 0 to 1
        metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec)
        const override {
            vec3 reflected = reflect(r_in.direction(), rec.normal); // reflected ray
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector()); // add fuzziness
            // scattered = ray(rec.p, reflected, r_in.time());
            // attenuation = albedo;
            // return (dot(scattered.direction(), rec.normal) > 0);
            srec.attenuation = albedo;
            srec.pdf_ptr = nullptr;
            srec.skip_pdf = true;
            srec.skip_pdf_ray = ray(rec.p, reflected, r_in.time());

            return true;
        }


    private:
        color albedo;
        double fuzz;
};



class dielectric : public material {
    public:
        // initialize the dielectric material with the refractive index
        dielectric(double refraction_index) : refraction_index(refraction_index) {}

        // * rewrite the scatter function, describe the scattering of the dielectric material (infact is refraction)
        bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec)
        const override {
            // attenuation settings
            // attenuation = color(1.0, 1.0, 1.0); // 1 means no attenuation, means the material is transparent (glass, water, etc.)

            srec.attenuation = color(1.0, 1.0, 1.0);
            srec.pdf_ptr = nullptr;
            srec.skip_pdf = true;

            // Determine the value of the refraction index based on the relationship between the light ray and the surface normal
            // * If the light enters the medium from the outside (rec.front_face is true), use 1.0/refraction_index;
            // * If the light exits the medium from the inside, directly use refraction_index
            double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index; // refractive index ratio

            vec3 unit_direction = unit_vector(r_in.direction()); // unit direction of the incoming ray

            double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0); // cos(theta) of the angle between the ray and the normal
            double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta); // sin(theta) of the angle between the ray and the normal

            // ? check rtoneweekend 11.3. total internal reflection
            bool cannot_refract = ri * sin_theta > 1.0; // whether the ray can be refracted
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, ri) > random_double())
                direction = reflect(unit_direction, rec.normal); // reflected ray
            else
                direction = refract(unit_direction, rec.normal, ri); // refracted ray

            // Generate the refracted ray, with the origin at the intersection point rec.p, and direction as the calculated refraction direction
            // * refracted only in situation that the ray can be refracted, else, the ray is reflected
            // scattered = ray(rec.p, direction, r_in.time());
            srec.skip_pdf_ray = ray(rec.p, direction, r_in.time());

            return true; // return true, means the ray is scattered (refracted)
        }


    private:
        // Refractive index in vacuum or air, or the ratio of the material's refractive index over
        // the refractive index of the enclosing media
        double refraction_index;

        static double reflectance(double cosine, double refraction_index) {
            // * Use Schlick's approximation for reflectance
            // * Schlick's approximation is used to calculate the reflectance of the material
            // * The reflectance is calculated based on the angle between the ray and the normal
            // * and the refractive index of the material
            // ? check rtoneweekend 11.4. Schlick's approximation
            auto r0 = (1 - refraction_index) / (1 + refraction_index);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow((1 - cosine), 5);
        }
};

class diffuse_light : public  material {
    public:
        // * constructor for texture
        diffuse_light(shared_ptr<texture> tex) : tex(tex) {}

        // * constructor for color, create a solid color texture
        diffuse_light(const color& emit) : tex(make_shared<solid_color>(emit)) {}

        // * emiited function, returns the emission color of the material at the given point
        color emitted(
            const ray& r_in, const hit_record& rec, double u, double v, const point3& p
            ) const  override {
                if (!rec.front_face)
                    return color(0, 0, 0); // return black if the ray is not facing the surface
            return tex->value(u, v, p);
        }


    private:
        shared_ptr<texture> tex;
};

class isotropic : public material {
  public:
    // Constructor that initializes the texture with a solid color
    isotropic(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}

    // Constructor that initializes the texture with a given texture
    isotropic(shared_ptr<texture> tex) : tex(tex) {}

    // Scatter function: simulates isotropic scattering
    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec)
    const override {
        // Scatter the ray in a random direction
        // scattered = ray(rec.p, random_unit_vector(), r_in.time());
        
        // Attenuate the ray based on the texture's color at the hit point
        // attenuation = tex->value(rec.u, rec.v, rec.p);
        // pdf = 1.0 / (4 * pi); // Set the PDF value to 1/(4*pi) for isotropic scattering
        srec.attenuation = tex->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = make_shared<sphere_pdf>();
        srec.skip_pdf = false;
        return true;
    }

    double scattering_pdf(
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const override {
        return 1.0 / (4 * pi); // Set the PDF value to 1/(4*pi) for isotropic scattering
    }

  private:
    shared_ptr<texture> tex;  // The texture that defines the material's color or pattern
};

#endif