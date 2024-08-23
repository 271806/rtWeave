#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

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

#endif