#include "geometry.hpp"

namespace {


using pvec3 = projector::vec3;


bool point_inside_plane(const pvec3& point, const pvec3& plane_p, const pvec3& normal) {
    pvec3 temp = point - plane_p;
    double dist = temp.dot(normal);
    return dist < 0;
}


bool point_inside_box(const pvec3& point, const pvec3& min, const pvec3& max) {
    for (int i = 0; i < 3; ++i) {
        if (!(point[i] > min[i] && point[i] < max[i])) {
            return false;
        }
    }
    return true;
}


bool point_inside_ellipsoid(const pvec3& point, const pvec3& center, const pvec3& radii) {
    pvec3 shifted = point - center;
    double a = (shifted[0] * shifted[0]) / (radii[0] * radii[0]);
    double b = (shifted[1] * shifted[1]) / (radii[1] * radii[1]);
    double c = (shifted[2] * shifted[2]) / (radii[2] * radii[2]);
    return (a + b + c) < 1;
}

} //annonymous namespace

namespace projector {


bool geom_primitive::point_is_inside(const vec3& point) const {
    switch (type) {
        case shape::plane:
            return point_inside_plane(point, param1, param2);
        case shape::aa_box:
            return point_inside_box(point, param1, param2);
        case shape::aa_ellipsoid:
            return point_inside_ellipsoid(point, param1, param2);
        default:
            return false;
    }
    return false;
}


bool operation::point_is_inside(const vec3& point) const {
    bool left_result = left->point_is_inside(point);
    bool right_result = right ->point_is_inside(point);
    switch(op) {
        case type::nop:
            return false;
        case type::join:
            return left_result || right_result;
        case type::intersect:
            return left_result && right_result;
        case type::cut:
            return left_result && !right_result;
        default:
            return false;
    }
    return false;
}


bool geometry::point_is_inside(const vec3& point) const {
    return std::visit([point](auto&& arg) {

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, geom_primitive>) {
            return arg.point_is_inside(point);
        } else if constexpr (std::is_same_v<T, operation>) {
            return arg.point_is_inside(point);
        }

        return false;
    }, definition);
}


}