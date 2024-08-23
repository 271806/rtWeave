#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h" // include ray.h for ray class

// hit_record is use for recording the hit point and normal of the object
class hit_record {
    public:
        point3 p; // hit point
        vec3 normal; // normal of the object
        double t; // t value of the hit point
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