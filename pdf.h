#ifndef PDF_H
#define PDF_H

#include "hittable.h"
#include "onb.h"


class pdf {
    public:
        virtual ~pdf() {}

        // Pure virtual function that returns the PDF value for a given direction
        virtual double value(const vec3& direction) const = 0;
        // Pure virtual function that generates a random direction based on the PDF distribution
        virtual vec3 generate() const = 0;
};

// Uniform density over the unit sphere
class sphere_pdf : public pdf {
    public:
        // Constructor
        sphere_pdf() {}

        // Override the value function to return the PDF value for a uniform distribution over the sphere
        double value(const vec3& direction) const override {
            return 1 / (4 * pi); // Uniform PDF value over the unit sphere
        }

        // Override the generate function to return a random direction uniformly distributed on the unit sphere
        vec3 generate() const override {
            return random_unit_vector(); // Generate a random unit vector
        }
};

// Cosine-weighted density
class cosine_pdf : public pdf {
    public:
        // Constructor that initializes the ONB with the given vector 'w'
        cosine_pdf(const vec3& w) : uvw(w) {}

        // Override the value function to return the cosine-weighted PDF value for a given direction
        double value(const vec3& direction) const override {
            auto cosine_theta = dot(unit_vector(direction), uvw.w());
            return std::fmax(0, cosine_theta/pi); // Return the cosine-weighted PDF value, ensuring non-negative
        }

        // Override the generate function to return a random direction based on the cosine-weighted distribution
        vec3 generate() const override {
            return uvw.transform(random_cosine_direction()); // Transform a random cosine direction to the world space
        }

    private:
        onb uvw; // Orthonormal basis used for transforming directions
};


class hittable_pdf : public pdf {
  public:
    hittable_pdf(const hittable& objects, const point3& origin)
      : objects(objects), origin(origin)
    {}

    double value(const vec3& direction) const override {
        return objects.pdf_value(origin, direction);
    }

    vec3 generate() const override {
        return objects.random(origin);
    }

  private:
    const hittable& objects;
    point3 origin;
};

#endif