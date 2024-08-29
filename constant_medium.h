#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable {
  public:
    // Constructor: Initializes the volume's boundary, density, and phase function
    constant_medium(shared_ptr<hittable> boundary, double density, shared_ptr<texture> tex)
      : boundary(boundary), neg_inv_density(-1/density),
        phase_function(make_shared<isotropic>(tex))
    {}

    // Overloaded constructor: Allows using color directly instead of a texture
    constant_medium(shared_ptr<hittable> boundary, double density, const color& albedo)
      : boundary(boundary), neg_inv_density(-1/density),
        phase_function(make_shared<isotropic>(albedo))
    {}

    // Hit function: Determines if a ray intersects with the volume
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record rec1, rec2;

        // Check if the ray enters the boundary
        if (!boundary->hit(r, interval::universe, rec1))
            return false;

        // Check if the ray exits the boundary
        if (!boundary->hit(r, interval(rec1.t+0.0001, infinity), rec2))
            return false;

        // Ensure the intersection points are within the specified time range
        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

        // If the first intersection point is after the second, there is no valid intersection
        if (rec1.t >= rec2.t)
            return false;

        // If the first intersection time is less than 0, adjust it to 0
        if (rec1.t < 0)
            rec1.t = 0;

        // Calculate the ray's length and the distance it travels inside the boundary
        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density * std::log(random_double());

        // If the distance inside the boundary is less than the scattering distance, no hit occurs
        if (hit_distance > distance_inside_boundary)
            return false;

        // Calculate the hit point where scattering occurs
        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        rec.normal = vec3(1,0,0);  // The normal vector is arbitrary as the volume has no explicit surface
        rec.front_face = true;     // The front face is also arbitrary
        rec.mat = phase_function;

        return true;
    }

    // Bounding box function: Returns the bounding box of the volume
    aabb bounding_box() const override { return boundary->bounding_box(); }

  private:
    shared_ptr<hittable> boundary;  // Defines the boundary of the volume
    double neg_inv_density;         // The negative inverse of the density, used to calculate scattering distance
    shared_ptr<material> phase_function;  // The phase function that determines scattering direction
};

#endif