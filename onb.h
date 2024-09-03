/* Orthonormal basis class */
#include "vec3.h"

#ifndef ONB_H
#define ONB_H

class onb {
  public:
    // Constructor: Takes a normal vector 'n' and constructs an orthonormal basis (ONB)
    onb(const vec3& n) {
        // The w-axis is aligned with the given normal vector 'n'
        axis[2] = unit_vector(n);
        // Choose an arbitrary vector 'a' that is not parallel to 'n'
        vec3 a = (std::fabs(axis[2].x()) > 0.9) ? vec3(0,1,0) : vec3(1,0,0);
        // The v-axis is perpendicular to both 'w' (the normal vector) and 'a'
        axis[1] = unit_vector(cross(axis[2], a));
        // The u-axis is perpendicular to both 'w' and 'v'
        axis[0] = cross(axis[2], axis[1]);
    }

    // Accessor functions for the orthonormal basis vectors
    const vec3& u() const { return axis[0]; }
    const vec3& v() const { return axis[1]; }
    const vec3& w() const { return axis[2]; }

    // Transform a vector 'v' from local space to world space using the ONB
    vec3 transform(const vec3& v) const {
        // Transform from basis coordinates to local space.
        return (v[0] * axis[0]) + (v[1] * axis[1]) + (v[2] * axis[2]);
    }

  private:
    vec3 axis[3]; // Array to store the u, v, and w axes of the ONB
};


#endif