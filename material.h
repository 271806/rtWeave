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
        metal(const color& albedo) : albedo(albedo) {}

        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
            vec3 reflected = reflect(r_in.direction(), rec.normal); // reflected ray
            scattered = ray(rec.p, reflected);
            attenuation = albedo;
            return true;
        }


    private:
        color albedo;
};

#endif