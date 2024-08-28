#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb { // * axis-aligned bounding box
    public:
        interval x, y, z; // * x, y, z intervals

        aabb() {} // * default constructor

        aabb(const interval& x, const interval& y, const interval& z)
         : x(x), y(y), z(z) {
            pad_to_minimums(); // * pad the bounding box to minimums
         } // * constructor with x, y, z intervals

        aabb(const point3& a, const point3& b) {
            // * constructor with two points
            // * the order of the points not matter
            // x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
            // y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
            // z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);

            // * Treat the two points a and b as extrema for the bounding box, so we don't require a
            // * particular minimum/maximum coordinate order.
            x = interval(std::fmin(a[0],b[0]), std::fmax(a[0],b[0]));
            y = interval(std::fmin(a[1],b[1]), std::fmax(a[1],b[1]));
            z = interval(std::fmin(a[2],b[2]), std::fmax(a[2],b[2]));

            pad_to_minimums(); // * pad the bounding box to minimums
        }

        aabb(const aabb& box0, const aabb& box1) {
            x= interval(box0.x, box1.x);
            y= interval(box0.y, box1.y);
            z= interval(box0.z, box1.z);
        }

        // return specific axis interval
        const interval& axis_interval(int n) const {
            if (n == 1) return y;
            if (n == 2) return z;
            return x;
        }

        // * check if the ray hits the bounding box
        bool hit(const ray& r, interval ray_t) const {
            const point3& ray_orig = r.origin(); // * ray origin
            const vec3& ray_dir = r.direction(); // * ray direction

            for (int axis = 0; axis < 3; axis++) { // * check each axis
                const interval& ax = axis_interval(axis); // get the axis interval
                const double adinv = 1.0 / ray_dir[axis]; // ray direction inverse

                auto t0 = (ax.min - ray_orig[axis]) * adinv;
                auto t1 = (ax.max - ray_orig[axis]) * adinv;

                // make sure t0 is mintime and t1 is maxtime
                if (t0 < t1) {
                    if (t0 > ray_t.min) ray_t.min = t0;
                    if (t1 < ray_t.max) ray_t.max = t1;
                } else {
                    if (t1 > ray_t.min) ray_t.min = t1;
                    if (t0 < ray_t.max) ray_t.max = t0;
                }

                if (ray_t.max <= ray_t.min) // if tmax <= tmin, no hit
                    return false;

            }
            return true;
        }

        int longest_axis() const {
            // returns the index of the longest axis of the bounding box

            if (x.size() > y.size())
                return x.size() > z.size() ? 0 : 2;
            else
                return y.size() > z.size() ? 1 : 2;
        }

        static const aabb empty, universe; // * empty and universe bounding box
    
    private:
        void pad_to_minimums() {
            // * Adjust the AABB so that no side is narrower than some delta, padding if necessary.

            double delta = 0.0001;
            // Padding
            if (x.size() < delta) x = x.expand(delta);
            if (y.size() < delta) y = y.expand(delta);
            if (z.size() < delta) z = z.expand(delta);
        }

};

const aabb aabb::empty    = aabb(interval::empty,    interval::empty,    interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);


// * The aabb + offset operator
aabb operator+(const aabb& bbox, const vec3& offset) {
    return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

aabb operator+(const vec3& offset, const aabb& bbox) {
    return bbox + offset;
}

#endif