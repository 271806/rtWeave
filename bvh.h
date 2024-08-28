#ifndef BVH_H
#define BVH_H

#include "rtweekend.h"

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

class bvh_node : public hittable { // * define bvh node representation from hittable
    public:
        bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {} // * constructor with hittable list
        // There's a C++ subtlety here. This constructor (without span indices) creates an
        // implicit copy of the hittable list, which we will modify. The lifetime of the copied
        // list only extends until this constructor exits. That's OK, because we only need to
        // persist the resulting bounding volume hierarchy.

        bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
            // Build the bounding box of the span of source objects
            bbox = aabb::empty;
            for (size_t object_index = start; object_index < end; object_index++) {
                bbox = aabb(bbox, objects[object_index]->bounding_box());
            }
            
            // int axis = random_int(0, 2); // randomly choose an axis (0, 1, 2): (x, y, z)
            int axis = bbox.longest_axis(); // choose the longest axis of the bounding box
            
            // compare function for sorting the objects (based on the specified axis)
            auto comparator = (axis == 0) ? box_x_compare
                            : (axis == 1) ? box_y_compare
                                          : box_z_compare;

            size_t object_span = end - start; // number of objects in the span

            if (object_span == 1) { // if there is only one object
                left = right = objects[start]; //put the object in both left and right child
            } else if (object_span == 2) { // if there are two objects
                left = objects[start]; // put the first object in the left child
                right = objects[start+1]; // put the second object in the right child
            } else {
                // if there are more than two objects, sort the objects based on the axis
                std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);

                auto mid = start + object_span/2; // find the middle index of the span (a simple way)
                left = make_shared<bvh_node>(objects, start, mid); //recursively build the left child
                right = make_shared<bvh_node>(objects, mid, end); // recursively build the right child
            }

            // ! Deprecated: combine the bounding box of the left and right child to get the bounding box of the current node
            // bbox = aabb(left->bounding_box(), right->bounding_box());
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // * if the ray does not hit the bounding box, return false
            if (!bbox.hit(r, ray_t)) return false;

            // if hit, check the left and right child
            bool hit_left = left->hit(r, ray_t, rec); // check left child
            // reduce the interval of the ray to the hit point of the left child
            bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec); // check right child

            return hit_left || hit_right; // return true if either left or right child is hit
        }


        // * bounding box of the bvh node, return current bounding box
        aabb bounding_box() const override {
            return bbox; // return the bounding box
        }
    
    private:
        shared_ptr<hittable> left; // left child
        shared_ptr<hittable> right; // right child
        aabb bbox; // bounding box of the current node

        // * general compare function for sorting the objects based on the axis (x, y or z)
        static bool box_compare(
            const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index
        ) {
            // obtain the interval of the objects on the specified axis
            auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
            auto b_axis_interval = b->bounding_box().axis_interval(axis_index);

            // * compare the minimum coordinate of both objects, return if a is more "ahead" than b
            return a_axis_interval.min < b_axis_interval.min;
        }

        // specified compare function of X0 Y1 Z2 axis
        static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 0);
        }

        static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 1);
        }

        static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 2);
        }
};

#endif