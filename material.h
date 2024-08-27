#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"

class hit_record; // will be used in the material class

class material {
    public:
        // virtual
        virtual ~material() = default;


        // * scatter the ray
        // r_in: the incoming ray
        // rec: the hit record
        // attenuation: the attenuation of the ray (attenutation means: the decrease in intensity of the light)
        // scattered: the scattered ray
        // return: whether the ray is scattered
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const {
            return false; // return false by default, means no scattering
        }

};


// define the lambertian material
class lambertian : public material {
    public:
        // initialize the lambertian material with albedo
        lambertian(const color& albedo) : albedo(albedo) {} // constructor with albedo
        
        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            if (scatter_direction.near_zero())
                // if the scatter direction is near zero, set it to the normal
                // to prevent the scatter direction to be zero
                scatter_direction = rec.normal;
            scattered = ray(rec.p, scatter_direction);

            attenuation = albedo;

            return true;
        }
    
    private:
        color albedo; // albedo of the material
};


class metal : public material {
    public:
        // take albedo and fuzziness
        // fuzziness is in 0 to 1
        metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
            vec3 reflected = reflect(r_in.direction(), rec.normal); // reflected ray
            reflected = unit_vector(reflected) + (fuzz * random_unit_vector()); // add fuzziness
            scattered = ray(rec.p, reflected);
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
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
        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
            // attenuation settings
            attenuation = color(1.0, 1.0, 1.0); // 1 means no attenuation, means the material is transparent (glass, water, etc.)

            // Determine the value of the refraction index based on the relationship between the light ray and the surface normal
            // * If the light enters the medium from the outside (rec.front_face is true), use 1.0/refraction_index;
            // * If the light exits the medium from the inside, directly use refraction_index
            double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index; // refractive index ratio

            vec3 unit_direction = unit_vector(r_in.direction()); // unit direction of the incoming ray
            vec3 refracted = refract(unit_direction, rec.normal, ri); // refracted ray
            // Generate the refracted ray, with the origin at the intersection point rec.p, and direction as the calculated refraction direction
            scattered = ray(rec.p, refracted);

            return true; // return true, means the ray is scattered (refracted)
        }


    private:
        // Refractive index in vacuum or air, or the ratio of the material's refractive index over
        // the refractive index of the enclosing media
        double refraction_index;
};

#endif