#ifndef VEC3_H
#define VEC3_H

// #include <cmath>
// #include <iostream>
#include "rtweekend.h"

//  ------------define a class 'vec3' that represents a 3D vector------------

class vec3 {
    public:
    double e[3]; // 3 elements of the vector

    vec3()  : e{0, 0, 0} {} // default constructor, default values are 0

    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {} // constructor with 3 arguments, default values are e0, e1, e2

    // define the x, y, z components of the vector
    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    // Overload operators for vector arithmetic

    // -vec3 operator
    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }

    // [] operator, for reading
    double operator[](int i) const { return e[i]; }

    // & operator, for reading and writing
    double& operator[](int i) { return e[i]; }

    // += operator, for adding two vectors and storing the result in the first vector
    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    // *= operator, for multiplying two vectors and storing the result in the first vector
    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    // /= operator, for dividing two vectors and storing the result in the first vector
    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    // length of the vector
    double length() const {
        return std::sqrt(length_squared());
    }

    // square of the length of the vector
    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    bool near_zero() const {
        // return true if the vector is close to zero in all dimensions
        auto s = 1e-8;
        return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }


    // * random vector generatation for diffuse materials
    static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

using point3 = vec3; // 3D point




// ----------------helper functions----------------

// overload << operator to print the vector
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// overload + operator to add two vectors
inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}


// overload - operator to subtract two vectors 
inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

// overload * operator to multiply two vectors
inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// overload * operator to multiply a vector with a scalar
inline vec3 operator*(double t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

// overload * operator to multiply a vector with a scalar
inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

// overload / operator to divide a vector by a scalar
inline vec3 operator/(const vec3 v, double t) {
    return (1/t) * v;
}

// overload dot product operator
inline double dot (const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
            + u.e[1] * v.e[1]
            + u.e[2] * v.e[2];
}


// overload cross product operator
inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// unit vector
inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0); // 2D plane z = 0
        if (p.length_squared() < 1)
            return p;
    }
}

// * rejection method for random vector generation
// * delete point outside the unit sphere is to make sure the vector's distribution is uniform
inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    // calculate the cos(theta) of the angle between the ray and the normal
    // -uv represents the direction of the ray (opposite direction), dot(-uv, n) calculate the dot product
    // use std::fmin to prevent the cos(theta) from being greater than 1 (floating point error)
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);

    // calculate the perpendicular component of the ray
    // ? check the formula in the book (onweekend 11.2 Snell's Law)
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);

    // calculate the parallel component of the ray
    // std::sqrt is used to calculate the sin(theta) of the angle between the ray and the normal
    // std::fabs is used to calculate the absolute value of the sin(theta)
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;

    return r_out_perp + r_out_parallel; // * return the refracted ray (perpendicular + parallel)
}

#endif