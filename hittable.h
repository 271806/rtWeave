#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h" // include ray.h for ray class

// hit_record is use for recording the hit point and normal of the object
class hit_record {
    public:
        point3 p; // hit point
        vec3 normal; // normal of the object
        double t; // t value of the hit point
        // * record if the hit point is in the front face of the object
        bool front_face; // front face of the object // * refer other liturature for more information (or alternative implementation)

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Sets the hit record normal vector.
            // * NOTE: the parameter `outward_normal` is assumed to have unit length.

            // * if the dot product of the ray direction and the outward normal is less than 0, the hit point is in the front face
            front_face = dot(r.direction(), outward_normal) < 0;

            // if is front face, the normal is the outward normal, otherwise, the normal is the negative of the outward normal
            normal = front_face ? outward_normal : -outward_normal;
        }
};


// hittable is a class for objects that can be hit by a ray
class hittable {
    public:
        // ? virtual destructor (need to learn more)
        virtual ~hittable() = default; // virtual destructor

        // virtual function hit, return true if the ray hits the object
        // r:ray
        // ray_tmin, ray_tmax: the range of t value of the ray
        // rec: hit_record
        virtual bool hit (const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
        // * `= 0` means this function is a pure virtual function, which means this function must be implemented in the derived class

};

#endif