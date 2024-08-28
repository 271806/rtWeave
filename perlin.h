#ifndef PERLIN_H
#define PERLIN_H

#include "rtweekend.h"

class perlin {
  public:
    // Constructor: Initializes the random float array and permutation arrays.
    perlin() {
        // Initialize the random float array with random double values.
        for (int i = 0; i < point_count; i++) {
            randfloat[i] = random_double();
        }

        // Generate permutation arrays for x, y, and z dimensions.
        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    // // Function to generate noise based on a 3D point p.
    // double noise(const point3& p) const {
    //     // Scale the coordinates and take the integer part.
    //     auto i = int(4*p.x()) & 255;
    //     auto j = int(4*p.y()) & 255;
    //     auto k = int(4*p.z()) & 255;

    //     // Combine the values from the permutation arrays and return the corresponding random float.
    //     return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
    // }

    double noise(const point3& p) const {
        // Compute the local coordinates u, v, w within the unit cube
        // These are fractional parts of p's coordinates and represent the point's position within the unit cube
        auto u = p.x() - std::floor(p.x());
        auto v = p.y() - std::floor(p.y());
        auto w = p.z() - std::floor(p.z());

        // Improvement with Hermitian Smoothing (use a Hermite cubic to round off the interpolation)
        u = u*u*(3-2*u);
        v = v*v*(3-2*v);
        w = w*w*(3-2*w);

        // Compute the unit cube's integer corner coordinates i, j, k
        auto i = int(std::floor(p.x()));
        auto j = int(std::floor(p.y()));
        auto k = int(std::floor(p.z()));

        // Initialize a 3D array c to store the random values at the cube's corners
        double c[2][2][2];

        // Loop through the 8 corners of the unit cube
        for (int di=0; di < 2; di++)
            for (int dj=0; dj < 2; dj++)
                for (int dk=0; dk < 2; dk++)
                    c[di][dj][dk] = randfloat[
                        perm_x[(i+di) & 255] ^
                        perm_y[(j+dj) & 255] ^
                        perm_z[(k+dk) & 255]
                    ];
        
        // * Perform trilinear interpolation to get a smooth noise value within the unit cube
        return trilinear_interp(c, u, v, w);
    }

  private:
    static const int point_count = 256; // Constant representing the number of points (256).
    double randfloat[point_count];  // Array to store random float values.
    int perm_x[point_count];  // Permutation array for the x dimension.
    int perm_y[point_count];  // Permutation array for the y dimension.
    int perm_z[point_count];  // Permutation array for the z dimension.

    // Function to generate a permutation array.
    static void perlin_generate_perm(int* p) {
        // Initialize the array with values from 0 to 255.
        for (int i = 0; i < point_count; i++)
            p[i] = i;

        // Randomly shuffle the array elements.
        permute(p, point_count);
    }

    // Function to shuffle an array randomly.
    static void permute(int* p, int n) {
        // Shuffle the array elements using the Fisher-Yates shuffle algorithm.
        for (int i = n-1; i > 0; i--) {
            int target = random_int(0, i);  // Generate a random index.
            int tmp = p[i];  // Swap the current element with the element at the random index.
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    // Trilinear interpolation function to smooth the noise values within the unit cube
    static double trilinear_interp(double c[2][2][2], double u, double v, double w) {
        auto accum = 0.0; // Accumulator to sum the interpolation results
        
        // Loop through the 8 corners of the unit cube
        for (int i=0; i < 2; i++)
            for (int j=0; j < 2; j++)
                for (int k=0; k < 2; k++)
                    // Compute the weighted value for each corner and accumulate the result
                    // (i*u + (1-i)*(1-u)) is the linear interpolation weight in the u direction
                    // (j*v + (1-j)*(1-v)) is the linear interpolation weight in the v direction
                    // (k*w + (1-k)*(1-w)) is the linear interpolation weight in the w direction
                    accum += (i*u + (1-i)*(1-u))
                        * (j*v + (1-j)*(1-v))
                        * (k*w + (1-k)*(1-w))
                        * c[i][j][k]; // Multiply the corner's random value by its weight

        return accum; // Return the final interpolated result
    }
};

#endif
