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

        static const interval empty, universe; // empty and universe interval


};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif