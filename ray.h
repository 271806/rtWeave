#ifndef RAY_H
#define RAY_H

#include "vec3.h"

// define a class 'ray' that represents a ray in 3D space
class ray {
    public:
        // default constructor, no initialization
        ray() {}

        // constructor with origin and direction
        ray(const point3& origin, const vec3& direction, double time)
         : orig(origin), dir(direction), tm(time) {}

        ray(const point3& origin, const vec3& direction)
         : ray(origin, direction, 0) {} // * default time is 0

        // getter functions for the origin and direction of the ray
        const point3& origin() const { return orig; }
        const vec3& direction() const { return dir; }


        // return the time of the ray
        double time() const { return tm; }

        // function to get the point on the ray at a distance t
        point3 at(double t) const {
            return orig + t*dir;
        }


    private:
        // origin of the ray
        point3 orig;

        // direction of the ray
        vec3 dir;

        // time of the ray
        double tm;
};


#endif