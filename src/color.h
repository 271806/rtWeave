#ifndef CLOROR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"
#include "rtweekend.h"

#include <algorithm> // std::clamp


using color = vec3;

// * gamma correction (gamma 2)
inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

inline double linear_to_gamma_adjustable(double linear_component, double gamma) {
    if (linear_component > 0)
        return pow(linear_component, 1.0 / gamma);  // 应用可调的gamma值

    return 0;
}


// define a function that writes the color values to the output stream
void write_color(std::ostream& out, const color& pixel_color) {
    // obtain the red, green, blue values of the pixel color
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

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


void write_color_png(unsigned char* pixel_data, int index, color pixel_color, int samples_per_pixel, double gamma) {
    // Apply adjustable gamma correction for each color component
    auto scale = 1.0 / samples_per_pixel;
    auto r = pow(pixel_color.x() * scale, 1.0 / gamma);
    auto g = pow(pixel_color.y() * scale, 1.0 / gamma);
    auto b = pow(pixel_color.z() * scale, 1.0 / gamma);

    // Clamp and convert to byte
    pixel_data[index] = static_cast<int>(256 * std::clamp(r, 0.0, 0.999));
    pixel_data[index + 1] = static_cast<int>(256 * std::clamp(g, 0.0, 0.999));
    pixel_data[index + 2] = static_cast<int>(256 * std::clamp(b, 0.0, 0.999));
}




#endif