/*2D quadrilateral (parallelogram) class
check out the alternate-2D-primitives tag in the source repository. This has solutions for triangles, ellipses and annuli (rings) in src/TheNextWeek/quad.h
*/
#ifndef QUAD_H
#define QUAD_H

#include "rtweekend.h"

#include "hittable.h"

class quad : public hittable {
    public:
        quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
         : Q(Q), u(u), v(v), mat(mat) {
            auto n = cross(u, v); // * normal of the quad
            normal = unit_vector(n); // * unit normal of the quad
            D = dot(Q, normal); // * D = dot(Q, n)
            w = n / dot(n, n); // * w = n / dot(n, n)
            
            set_bouding_box();
        }

        virtual void set_bouding_box() {
            // * Compute the bounding box of all four vertices.
            // this caluclates the bounding box of the quad
            // * two diagonals of the quad
            auto bbox_diagonal1 = aabb(Q, Q + u + v);
            auto bbox_diagonal2 = aabb(Q + u, Q + v);
            // * bounding box of the quad is the bounding box of the two diagonals
            bbox = aabb(bbox_diagonal1, bbox_diagonal2);
        }

        aabb bounding_box() const override {
            return bbox;
        }

        // hit detection
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            auto denom = dot(normal, r.direction()); // * denominator of the ray-plane intersection

            // No hit if the ray is parallel to the plane
            if (std::fabs(denom) < 1e-8) return false;

            // return false if the hit point parameter t is outside the ray interval
            auto t = (D - dot(normal, r.origin())) / denom;
            if(!ray_t.contains(t)) return false;

            // * Determine if the hit point lies within the planar shape using its plane coordinates.

            auto intersection = r.at(t); // * intersection point

            vec3 planar_hitpt_vector = intersection - Q; // * vector from the origin of the quad to the hit point

            // calculate alpha and beta for checking if the hit point lies within the quad
            auto alpha = dot(w, cross(planar_hitpt_vector, v));
            auto beta = dot(w, cross(u, planar_hitpt_vector));

            if(!is_interior(alpha, beta, rec)) return false; // return false if the hit point is not in the interior


            // * if hit the 2D plane
            // auto intersection = r.at(t); // * intersection point

            // * Ray hits the 2D shape; set the rest of the hit record and return true
            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;
        }

        virtual bool is_interior(double a, double b, hit_record& rec) const {
            // Given the hit point in plane coordinates, return false if it is outside the
            // primitive, otherwise set the hit record UV coordinates and return true.
            interval unit_interval(0, 1);

            // * check if the hit point is in the interior (if a or b is in 0 to 1)
            if (!unit_interval.contains(a) || !unit_interval.contains(b)) return false;

            // * if is in the interior, set the UV coordinates
            rec.u = a;
            rec.v = b;
            return true;
        }

    private:
        point3 Q; // * origin of the quad
        vec3 u, v; // * two vectors that define the quad
        vec3 w;
        shared_ptr<material> mat; // * material of the quad
        aabb bbox; // * bounding box of the quad
        
        vec3 normal;
        double D;
};

#endif