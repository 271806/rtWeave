#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

// ? learn more about how to use below (features of C++)
// ? check for https://raytracing.github.io/books/RayTracingInOneWeekend.html#addingasphere, section 6.6. (version 4.0.0)
// #include <memory> // include memory for shared_ptr
#include <vector> // include vector for std::vector

// ? the using statements in listing 21 tell the compiler that we'll be getting shared_ptr and make_shared from the std library,
// ? so we don't need to prefix these with std:: every time we reference them.
// ! Deprecated (include in rtweekend.h)
// using std::shared_ptr;
// using std::make_shared; 

class hittable_list : public hittable {
    public:
        std::vector<shared_ptr<hittable>> objects; // vector of shared_ptr of hittable objects

        hittable_list() {} // default constructor

        hittable_list(shared_ptr<hittable> object) { add(object); } // constructor with shared_ptr of hittable object

        void clear() { objects.clear(); } // clear the objects vector

        void add(shared_ptr<hittable> object) { 
            objects.push_back(object); // add a shared_ptr of hittable object to the objects vector
        }

        // check if the ray hits the object in the list
        // if hit: true, then record the hit info
        bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
            hit_record temp_rec; // temporary hit record
            bool hit_anything = false; // flag for if hit or not

            // tmax is the maximum t value of the ray
            // * record the closest hit point
            auto closest_so_far = ray_tmax; // initialize the closest_so_far to the maximum t value

            // loop through all the objects in the list
            for (const auto& object : objects) {
                // if the ray hits the object
                if (object->hit(r, ray_tmin, closest_so_far, temp_rec)) {
                    hit_anything = true; // set the hit flag to true
                    closest_so_far = temp_rec.t; // update the closest_so_far to the t value of the hit point
                    rec = temp_rec; // update the hit record
                }
            }

            return hit_anything; // return the hit flag
        }

};

#endif