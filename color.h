#ifndef CLOROR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"
#include "rtweekend.h"

using color = vec3;

// * gamma correction (gamma 2)
inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

// define a function that writes the color values to the output stream
void write_color(std::ostream& out, const color& pixel_color) {
    // obtain the red, green, blue values of the pixel color
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // apply gamma correction gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // scale [0, 1] to [0, 255]
    // multiply by 255.999 to accurately scale the values
    // * Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // output the color values to the output stream (usually a file or standard output)
    out << rbyte << " " << gbyte << " " << bbyte << "\n";
}

#endif