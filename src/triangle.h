#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"
#include <memory>

class Triangle : public hittable {
public:
    Triangle(const vec3& v0, const vec3& v1, const vec3& v2, std::shared_ptr<material> mat)
        : v0(v0), v1(v1), v2(v2), mat(mat) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    aabb bounding_box() const override;

private:
    vec3 v0, v1, v2;
    std::shared_ptr<material> mat;
};

// 实现 hit 函数
bool Triangle::hit(const ray& r, interval ray_t, hit_record& rec) const {
    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    vec3 h = cross(r.direction(), edge2);
    double a = dot(edge1, h);
    if (a > -1e-8 && a < 1e-8) return false;

    double f = 1.0 / a;
    vec3 s = r.origin() - v0;
    double u = f * dot(s, h);
    if (u < 0.0 || u > 1.0) return false;

    vec3 q = cross(s, edge1);
    double v = f * dot(r.direction(), q);
    if (v < 0.0 || u + v > 1.0) return false;

    double t = f * dot(edge2, q);
    if (ray_t.surrounds(t)) {
        rec.t = t;
        rec.p = r.at(t);
        vec3 outward_normal = unit_vector(cross(edge1, edge2));
        rec.set_face_normal(r, outward_normal);  // 确保法线方向正确
        rec.mat = mat;
        return true;
    }

    return false;
}

// bounding_box
aabb Triangle::bounding_box() const {
    vec3 min_point(
        std::fmin(v0.x(), std::fmin(v1.x(), v2.x())),
        std::fmin(v0.y(), std::fmin(v1.y(), v2.y())),
        std::fmin(v0.z(), std::fmin(v1.z(), v2.z()))
    );

    vec3 max_point(
        std::fmax(v0.x(), std::fmax(v1.x(), v2.x())),
        std::fmax(v0.y(), std::fmax(v1.y(), v2.y())),
        std::fmax(v0.z(), std::fmax(v1.z(), v2.z()))
    );

    return aabb(min_point, max_point);  // Use the minimum and maximum points to construct the bounding box
}

#endif
