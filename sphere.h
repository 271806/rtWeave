#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"


// sphere class
class sphere : public hittable {
    public:
        // initialize the sphere with center and radius
        // std::fmax(0, radius) is used to prevent negative radius
        sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

        // check if the ray hits the sphere, if hit, return the record
        // if hit: true, else: false
        bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
            vec3 oc = center - r.origin(); // vector from ray origin to the center of the sphere
            
            // * simplified dicriminant calculation
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius * radius;
            auto discriminant = h * h - a * c;

            if (discriminant < 0)
                return false;
            
            auto sqrtd = std::sqrt(discriminant); // calculate the square root of the discriminant


            // Find the nearest root that lies in the acceptable range.
            auto root = (h - sqrtd) / a; // * root_smaller
            if (root <= ray_tmin || root >= ray_tmax) {
                root = (h + sqrtd) / a; // * try root_larger
                if (root <= ray_tmin || root >= ray_tmax)
                    return false; // * return false if both roots are out of range
            }

            // if root is in the acceptable range, record the hit point and normal
            rec.t = root; // t value
            rec.p = r.at(rec.t); // hit point coordinate
            // rec.normal = (rec.p - center) / radius; // normal = (hitpoint - center) / radius (for a sphere)
            // * normal at the hit point (without front face check)
            vec3 outward_normal = (rec.p - center) / radius; // outward normal
            // * use set_face_normal from hit_record to set the normal's direction
            rec.set_face_normal(r, outward_normal); // set the face normal

            return true; // * return true if hit
        }

    private:
        point3 center;
        double radius;
};

#endif