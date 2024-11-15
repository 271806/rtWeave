#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h"

// interval class is used to represent a range of values
class interval {
    public:
        double min, max;

        // default constructor
        interval() : min(+infinity), max(-infinity) {} // default interval is empty

        // constructor with min and max values
        interval(double min, double max) : min(min), max(max) {}

        interval(const interval& a, const interval& b) {
           // Create the interval tightly enclosing the two input intervals
           min = a.min <= b.min ? a.min : b.min;
           max = a.max >= b.max ? a.max : b.max;
        }

        // calculate interval length
        double size() const { return max - min; }

        // check if a value x is in the interval (include the boundary)
        bool contains(double x) const {
            return min <= x && x<= max;
        }

        // check if a value x is in the interval (not include the boundary)
        bool surrounds(double x) const {
            return min < x && x < max;
        }

        double clamp(double x) const {
            if (x < min) return min;
            if (x > max) return max;
            return x;
        }

        interval expand(double delta) const {
            auto padding = delta / 2;
            // expand the interval by custum value (delta)
            return interval(min - padding, max + padding);
        }

        static const interval empty, universe; // empty and universe interval


};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

// * The interval + displacement operator
interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}

#endif