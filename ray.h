#ifndef RAY_H
#define RAY_H

#include "vec3.h"

// define a class 'ray' that represents a ray in 3D space
class ray {
    public:
        // default constructor, no initialization
        ray() {}

        // constructor with origin and direction
        ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {}

        // getter functions for the origin and direction of the ray
        const point3& origin() const { return orig; }
        const vec3& direction() const { return dir; }

        // function to get the point on the ray at a distance t
        point3 at(double t) const {
            return orig + t*dir;
        }


    private:
        // origin of the ray
        point3 orig;

        // direction of the ray
        vec3 dir;

};


#endif