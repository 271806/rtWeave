#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"
#include "aabb.h"


class material; // forward declaration of material class

// hit_record is use for recording the hit point and normal of the object
class hit_record {
    public:
        point3 p; // hit point
        vec3 normal; // normal of the object
        shared_ptr<material> mat; // material of the object
        double t; // t value of the hit point
        double u; // u coordinate of the hit point
        double v; // v coordinate of the hit point
        // * record if the hit point is in the front face of the object
        bool front_face; // front face of the object // * refer other liturature for more information (or alternative implementation)

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Sets the hit record normal vector.
            // * NOTE: the parameter `outward_normal` is assumed to have unit length.

            // * if the dot product of the ray direction and the outward normal is less than 0, the hit point is in the front face
            front_face = dot(r.direction(), outward_normal) < 0;

            // if is front face, the normal is the outward normal, otherwise, the normal is the negative of the outward normal
            normal = front_face ? outward_normal : -outward_normal;
        }
};


// hittable is a class for objects that can be hit by a ray
class hittable {
    public:
        // ? virtual destructor (need to learn more)
        virtual ~hittable() = default; // virtual destructor

        // virtual function hit, return true if the ray hits the object
        // r:ray
        // ray_tmin, ray_tmax: the range of t value of the ray
        // rec: hit_record
        // ! Deprecated: virtual bool hit (const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
        // * `= 0` means this function is a pure virtual function, which means this function must be implemented in the derived class

        virtual aabb bounding_box() const = 0; // for bvhtree
        
        virtual double pdf_value(const point3& origin, const vec3& direction
        ) const {
            return 0.0;
        }

        virtual vec3 random(const vec3& origin) const {
            return vec3(1, 0, 0);
        }

};

class translate : public hittable {
    public:
        // Constructor: takes an object and an offset, initializes the translate object
        translate(shared_ptr<hittable> object, const vec3& offset)
        : object(object), offset(offset)
        {
            // Calculate the bounding box of the translated object
            bbox = object->bounding_box() + offset;
        }


        // check the ray object intersection
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // * Translate the ray by the offset vector and check for intersection with the object.
            ray offset_r(r.origin() - offset, r.direction(), r.time());

            // Check if there is an intersection along the offset ray
            // (if there is an intersection, determine the position of the intersection)
            if (!object->hit(offset_r, ray_t, rec))
                return false;

            // Move the intersection point forward by the offset distance to place it on the path of the original ray
            rec.p += offset; // * update the hit point

            return true;
        }

        aabb bounding_box() const override {
            return bbox;
        }

    private:
        shared_ptr<hittable> object; // * object to be translated
        vec3 offset; // * offset of the translation
        aabb bbox; // * bounding box of the translated object
};

class rotate_x : public hittable {
public:
    rotate_x(shared_ptr<hittable> object, double angle) : object(object) {
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newy =  cos_theta*y - sin_theta*z;
                    auto newz =  sin_theta*y + cos_theta*z;

                    vec3 tester(x, newy, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[1] = cos_theta*r.origin()[1] - sin_theta*r.origin()[2];
        origin[2] = sin_theta*r.origin()[1] + cos_theta*r.origin()[2];

        direction[1] = cos_theta*r.direction()[1] - sin_theta*r.direction()[2];
        direction[2] = sin_theta*r.direction()[1] + cos_theta*r.direction()[2];

        ray rotated_r(origin, direction, r.time());

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        auto p = rec.p;
        p[1] =  cos_theta*rec.p[1] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[1] + cos_theta*rec.p[2];

        auto normal = rec.normal;
        normal[1] =  cos_theta*rec.normal[1] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[1] + cos_theta*rec.normal[2];

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};


class rotate_y : public hittable {
    public:
        rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }
    

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Change the ray from world space to object space
            auto origin = r.origin();
            auto direction = r.direction();

            origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
            origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

            direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
            direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

            ray rotated_r(origin, direction, r.time());

            // Determine whether an intersection exists in object space (and if so, where)
            if (!object->hit(rotated_r, ray_t, rec))
                return false;

            // Change the intersection point from object space to world space
            auto p = rec.p;
            p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
            p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

            // Change the normal from object space to world space
            auto normal = rec.normal;
            normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
            normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

            rec.p = p;
            rec.normal = normal;

            return true;
        }

        aabb bounding_box() const override { return bbox; }
    
    private:
        shared_ptr<hittable> object;
        double sin_theta;
        double cos_theta;
        aabb bbox;
};

class rotate_z : public hittable {
public:
    rotate_z(shared_ptr<hittable> object, double angle) : object(object) {
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x - sin_theta*y;
                    auto newy =  sin_theta*x + cos_theta*y;

                    vec3 tester(newx, newy, z);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[1];
        origin[1] = sin_theta*r.origin()[0] + cos_theta*r.origin()[1];

        direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[1];
        direction[1] = sin_theta*r.direction()[0] + cos_theta*r.direction()[1];

        ray rotated_r(origin, direction, r.time());

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        auto p = rec.p;
        p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[1];
        p[1] = -sin_theta*rec.p[0] + cos_theta*rec.p[1];

        auto normal = rec.normal;
        normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[1];
        normal[1] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[1];

        rec.p = p;
        rec.normal = normal;

        return true;
    }

    aabb bounding_box() const override { return bbox; }

private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};


#endif