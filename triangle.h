#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"
#include <memory>

class Triangle : public hittable {
public:
    Triangle(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& n0, const vec3& n1, const vec3& n2, std::shared_ptr<material> mat)
        : v0(v0), v1(v1), v2(v2), n0(n0), n1(n1), n2(n2), mat(mat) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;
    aabb bounding_box() const override;

private:
    vec3 v0, v1, v2;  // 三角形顶点
    vec3 n0, n1, n2;  // 三角形的每个顶点的法线
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

        // 使用插值计算法线（根据三角形的顶点法线进行插值）
        rec.normal = unit_vector((1 - u - v) * n0 + u * n1 + v * n2);

        // 设置正确的法线方向
        rec.set_face_normal(r, rec.normal);
        
        rec.mat = mat;
        return true;
    }

    return false;
}


#endif
