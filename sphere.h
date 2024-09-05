#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "onb.h"  // Include for Orthonormal Basis (ONB) transformations

// Sphere class that inherits from the hittable class
class sphere : public hittable {
  public:
    // Constructor for a stationary sphere
    // Takes a static center (point3), a radius, and a shared pointer to the material
    sphere(const point3& static_center, double radius, shared_ptr<material> mat)
      : center(static_center, vec3(0,0,0)), radius(std::fmax(0,radius)), mat(mat)
    {
        // Create a vector of radius in all directions (x, y, z) for bounding box calculation
        auto rvec = vec3(radius, radius, radius);
        // Create an axis-aligned bounding box (AABB) using the center and radius
        bbox = aabb(static_center - rvec, static_center + rvec);
    }

    // Constructor for a moving sphere
    // Takes two center positions (center1 and center2) representing the motion between time 0 and time 1,
    // a radius, and a shared pointer to the material
    sphere(const point3& center1, const point3& center2, double radius,
           shared_ptr<material> mat)
      : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat(mat)
    {
        // Create the bounding boxes for the sphere at time 0 and time 1
        auto rvec = vec3(radius, radius, radius);
        aabb box1(center.at(0) - rvec, center.at(0) + rvec); // Bounding box at time 0
        aabb box2(center.at(1) - rvec, center.at(1) + rvec); // Bounding box at time 1
        bbox = aabb(box1, box2); // Merge bounding boxes into a single bounding box
    }

    // Hit function that checks if a ray hits the sphere
    // It takes a ray, a time interval, and a hit record to store hit information
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Get the current center of the sphere based on the time of the ray
        point3 current_center = center.at(r.time());
        // Compute the vector from the ray's origin to the sphere's center
        vec3 oc = current_center - r.origin();
        // Compute quadratic coefficients for the sphere-ray intersection
        auto a = r.direction().length_squared(); // The squared length of the ray direction
        auto h = dot(r.direction(), oc);         // Projection of oc on the ray direction
        auto c = oc.length_squared() - radius*radius; // Difference between oc squared and radius squared

        // Calculate the discriminant to check if the ray intersects the sphere
        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false; // No intersection if the discriminant is negative

        // Calculate the square root of the discriminant (used to find the roots)
        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root within the acceptable range
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {  // If the root is not in the range, try the other root
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false; // If both roots are out of range, return false (no hit)
        }

        // If we have a valid root, update the hit record
        rec.t = root; // Record the hit time
        rec.p = r.at(rec.t); // Compute the hit point using the ray function
        // Compute the normal at the hit point and set the face normal
        vec3 outward_normal = (rec.p - current_center) / radius;
        rec.set_face_normal(r, outward_normal); // Adjust normal direction depending on the ray
        get_sphere_uv(outward_normal, rec.u, rec.v); // Calculate texture UV coordinates
        rec.mat = mat; // Assign the material to the hit record

        return true; // Return true indicating a hit
    }

    // Function to return the axis-aligned bounding box of the sphere
    aabb bounding_box() const override { return bbox; }

    // Function to calculate the probability density function (PDF) value
    // This is used to determine how likely a direction is to hit the sphere
    double pdf_value(const point3& origin, const vec3& direction) const override {
        // Create a temporary hit record
        hit_record rec;
        // Cast a ray from the origin in the given direction and check for hits
        if (!this->hit(ray(origin, direction), interval(0.001, infinity), rec))
            return 0; // If the ray doesn't hit the sphere, return 0

        // Compute the squared distance from the origin to the sphere's center
        auto dist_squared = (center.at(0) - origin).length_squared();
        // Compute the maximum cosine of the angle between the ray and the surface normal
        auto cos_theta_max = std::sqrt(1 - radius*radius/dist_squared);
        // Calculate the solid angle of the sphere from the origin
        auto solid_angle = 2*pi*(1-cos_theta_max);

        return  1 / solid_angle; // Return the inverse of the solid angle (the PDF value)
    }

    // Function to generate a random direction towards the sphere from a given origin
    vec3 random(const point3& origin) const override {
        // Compute the direction from the origin to the sphere's center
        vec3 direction = center.at(0) - origin;
        // Calculate the squared distance from the origin to the sphere
        auto distance_squared = direction.length_squared();
        // Use an orthonormal basis (ONB) to transform random directions into the local space
        onb uvw(direction);
        // Return a randomly generated direction to the sphere
        return uvw.transform(random_to_sphere(radius, distance_squared));
    }

  private:
    ray center;  // The ray represents the center of the sphere (static or moving)
    double radius;  // Radius of the sphere
    shared_ptr<material> mat;  // Pointer to the material of the sphere
    aabb bbox;  // The bounding box of the sphere

    // Utility function to calculate the UV coordinates of a point on the sphere
    static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p: A point on the surface of the sphere
        // u: Angle around the Y-axis, normalized between 0 and 1
        // v: Angle from Y = -1 to Y = +1, normalized between 0 and 1

        auto theta = std::acos(-p.y());  // Compute the polar angle (latitude)
        auto phi = std::atan2(-p.z(), p.x()) + pi;  // Compute the azimuthal angle (longitude)

        u = phi / (2*pi);  // Normalize the azimuthal angle
        v = theta / pi;    // Normalize the polar angle
    }

    // Utility function to generate a random direction towards the sphere
    static vec3 random_to_sphere(double radius, double distance_squared) {
        // Generate two random numbers between 0 and 1
        auto r1 = random_double();
        auto r2 = random_double();
        // Calculate the z-component of the random direction
        auto z = 1 + r2*(std::sqrt(1-radius*radius/distance_squared) - 1);

        // Compute the azimuthal angle (phi) and the x, y components
        auto phi = 2*pi*r1;
        auto x = std::cos(phi) * std::sqrt(1-z*z);
        auto y = std::sin(phi) * std::sqrt(1-z*z);

        // Return the random direction vector
        return vec3(x, y, z);
    }
};

#endif
