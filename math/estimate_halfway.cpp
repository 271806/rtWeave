#include "../rtweekend.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>

// * struct is a class with public members by default
struct sample {
    double x; // store x
    double p_x; // store p(x)
};


// for sorting samples by x
bool compare_by_x(const sample& a, const sample& b) {
    return a.x < b.x;
}

int main() {
    const unsigned int N = 10000; // Number of samples
    sample samples[N]; // Array of samples
    double sum = 0.0; // Sum of p(x) values

    // Iterate through all of our samples.

    for (unsigned int i = 0; i < N; i++) {
        // Get the area under the curve.
        auto x = random_double(0, 2*pi); // Random x in [0, 2pi]
        auto sin_x = std::sin(x); // sin(x)
        auto p_x = exp(-x / (2*pi)) * sin_x * sin_x; // p(x) = e^(-x/2pi) * sin(x)^2
        sum += p_x; // Add p(x) to the sum.

        // Store this sample.
        sample this_sample = {x, p_x};
        samples[i] = this_sample;
    }

    // Sort the samples by x.
    std::sort(std::begin(samples), std::end(samples), compare_by_x);

    // Find out the sample at which we have half of our area.
    double half_sum = sum / 2.0; // Half of the sum
    double halfway_point = 0.0; // The x value at which we have half of our area
    double accum = 0.0; // Accumulator for the sum
    for (unsigned int i = 0; i < N; i++){
        accum += samples[i].p_x;
        if (accum >= half_sum) {
            halfway_point = samples[i].x; // Store the halfway point
            break;
        }
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Average = " << sum / N << '\n';
    std::cout << "Area under curve = " << 2 * pi * sum / N << '\n';
    std::cout << "Halfway = " << halfway_point << '\n';
}