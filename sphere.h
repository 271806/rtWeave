#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
// #include "vec3.h"
#include "rtweekend.h"


// sphere class
class sphere : public hittable {
    public:
        // TODO: make stationary and moving spheres class the same (if not moving, center2 = center1)
        // * Stationary Sphere
        sphere(const point3& center, double radius, shared_ptr<material> mat)
         : center1(center), radius(std::fmax(0, radius)), mat(mat), is_moving(false) {
            auto rvec = vec3(radius, radius, radius); // a vector represent the radius on x, y, z axis
            bbox = aabb(center - rvec, center + rvec); // bounding box of the sphere, defining the box with two points
         }

        // * Moving Sphere
        sphere(const point3& center1, const point3& center2, double radius,
               shared_ptr<material> mat)
         : center1(center1), radius(std::fmax(0,radius)), mat(mat), is_moving(true)
        {
            auto rvec = vec3(radius, radius, radius); // a vector represent the radius on x, y, z axis
            aabb box1(center1 - rvec, center1 + rvec); // bounding box of the sphere at time 0
            aabb box2(center2 - rvec, center2 + rvec); // bounding box of the sphere at time 1

            bbox = aabb(box1, box2); // bounding box of the sphere, defining the box with two bounding boxes
            
            center_vec = center2 - center1; // calculate the vector from center1 to center2 (moving vector)
        }

        // initialize the sphere with center and radius
        // std::fmax(0, radius) is used to prevent negative radius
        // sphere(const point3& center, double radius, shared_ptr<material> mat) : center(center), radius(std::fmax(0, radius)), mat(mat) {}

        // check if the ray hits the sphere, if hit, return the record
        // if hit: true, else: false
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            point3 center = is_moving ? sphere_center(r.time()) : center1; // get the center of the sphere
            vec3 oc = center - r.origin(); // vector from ray origin to the center of the sphere
            
            // * simplified dicriminant calculation
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius * radius;
            auto discriminant = h * h - a * c;

            if (discriminant < 0)
                return false;
            
            auto sqrtd = std::sqrt(discriminant); // calculate the square root of the discriminant


            // Find the nearest root that lies in the acceptable range.
            auto root = (h - sqrtd) / a; // * root_smaller
            // ! Deprecated (not using the interval class)
            // if (root <= ray_tmin || root >= ray_tmax) {
            //     root = (h + sqrtd) / a; // * try root_larger
            //     if (root <= ray_tmin || root >= ray_tmax)
            //         return false; // * return false if both roots are out of range
            // }
            // ! Deprecated end

            // * using interval class
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a; // * try root_larger
                if (!ray_t.surrounds(root))
                    return false; // * return false if both roots are out of range
            }

            // if root is in the acceptable range, record the hit point and normal
            rec.t = root; // t value
            rec.p = r.at(rec.t); // hit point coordinate
            // rec.normal = (rec.p - center) / radius; // normal = (hitpoint - center) / radius (for a sphere)
            // * normal at the hit point (without front face check)
            vec3 outward_normal = (rec.p - center) / radius; // outward normal
            // * use set_face_normal from hit_record to set the normal's direction
            rec.set_face_normal(r, outward_normal); // set the face normal
            get_sphere_uv(outward_normal, rec.u, rec.v); // get the uv coordinate of the hit point
            rec.mat = mat; // set the material

            return true; // * return true if hit
        }

        aabb bounding_box() const override {
            return bbox;
        }

    private:
        point3 center1;
        // point3 center;
        double radius;
        shared_ptr<material> mat; // material of the sphere
        bool is_moving;
        vec3 center_vec;
        aabb bbox;

        point3 sphere_center(double time) const {
            // Linearly interpolate from center1 to center2 according to time, where t=0 yields
            // center1, and t=1 yields center2.
            return center1 + time * center_vec;
        }

        static void get_sphere_uv(const point3& p, double& u, double& v) {
            // p: a given point on the sphere of radius one, centered at the origin.
            // u: returned value [0,1] of angle around the Y axis from X=-1.
            // v: returned value [0,1] of angle from Y=-1 to Y=+1.
            //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
            //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
            //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
            auto theta = std::acos(-p.y());
            auto phi = std::atan2(-p.z(), p.x()) + pi;

            u = phi / (2*pi);
            v = theta / pi;
        }
};

#endif