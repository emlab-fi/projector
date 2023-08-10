#include "surface.hpp"

#include "constants.hpp"

#include <cmath>

namespace {

using pvec3 = projector::vec3;

//=======================================
// Generic functions to evaluate surfaces in this form:
//
//  S_x * (x^2 / a^2) + S_y * (y^2 / b^2) + S_z * (z^2 / c^2) = R
//
//=======================================

template <int S_x, int S_y, int S_z, int R>
double generic_eval(const pvec3 &p, double a, double b, double c) {
    double x = (S_x * p.x() * p.x()) / a * a;
    double y = (S_y * p.y() * p.y()) / b * b;
    double z = (S_z * p.z() * p.z()) / c * c;

    return (x + y + z) < R;
}


template <int S_x, int S_y, int S_z, int R>
double generic_dist(const pvec3 &p, const pvec3 &d, double a, double b,
                    double c) {

    double e = S_x * std::pow(b * c * d.x(), 2) +
               S_y * std::pow(a * c * d.y(), 2) +
               S_z * std::pow(a * b * d.z(), 2);

    double f = S_x * std::pow(b * c, 2) * 2 * p.x() * d.x() +
               S_y * std::pow(a * c, 2) * 2 * p.y() * d.y() +
               S_z * std::pow(a * b, 2) * 2 * p.z() * d.z();

    double g = S_x * std::pow(b * c * p.x(), 2) +
               S_y * std::pow(a * c * p.y(), 2) +
               S_z * std::pow(a * b * p.z(), 2) - R * std::pow(a * b * c, 2);

    // we now have e * t^2 + f * t + g = 0, solve it via discriminant

    // if e is zero, we never intersect the surface
    if (e == 0.0) {
        return projector::constants::infinity;
    }

    double discriminant = f * f - 4 * e * g;

    // if the discriminant is negative, we have only complex roots and
    // no intersect
    if (discriminant < 0.0) {
        return projector::constants::infinity;
    }

    double dist1 = (-f + std::sqrt(discriminant)) / (2 * e);
    double dist2 = (-f - std::sqrt(discriminant)) / (2 * e);

    // if both solutions are positive, return the smaller one
    if (dist1 > 0.0 && dist2 > 0.0) {
        return std::min(dist1, dist2);
    }

    // we have one positive and one negative solution, return the positive
    // or we have two negative, same situation
    return std::max(dist1, dist2);
}

} // namespace

namespace projector {

double plane::distance_along_line(const vec3 &p, const vec3 &dir) const {

    double denom = normal.dot(dir);

    // if the denominator is zero, the particle is travelling parallel
    // and will never intersect
    if (denom == 0.0) {
        return constants::infinity;
    }

    double numer = (-normal).dot(p - point);

    double d = numer / denom;

    return d;
}

bool plane::point_inside(const vec3 &p) const {
    vec3 temp = p - point;
    double dist = temp.dot(normal);
    return dist < 0;
}

std::pair<vec3, vec3> plane::bounding_box() const {
    constexpr double inf = constants::infinity;

    // plane perpendicular to z axis
    if (normal.x() == 0.0 && normal.y() == 0.0) {
        if (normal.z() < 0.0) {
            return {{-inf, -inf, point.z()}, {inf, inf, inf}};
        }
        return {{-inf, -inf, -inf}, {inf, inf, point.z()}};
    }

    // plane perpendicular to y axis
    if (normal.x() == 0.0 && normal.z() == 0.0) {
        if (normal.y() < 0.0) {
            return {{-inf, point.y(), -inf}, {inf, inf, inf}};
        }
        return {{-inf, -inf, -inf}, {inf, point.y(), inf}};
    }

    // plane perpendicular to x axis
    if (normal.y() == 0.0 && normal.z() == 0.0) {
        if (normal.x() < 0.0) {
            return {{point.x(), -inf, -inf}, {inf, inf, inf}};
        }
        return {{-inf, -inf, -inf}, {point.x(), inf, inf}};
    }

    // generic plane can't be bounded
    return {{-inf, -inf, -inf}, {inf, inf, inf}};
}

double ellipsoid::distance_along_line(const vec3 &p, const vec3 &dir) const {
    // shift the point so that the ellipsoid is at zero
    vec3 shifted = p - center;

    return generic_dist<1, 1, 1, 1>(shifted, dir, radii[0], radii[1], radii[2]);
}

bool ellipsoid::point_inside(const vec3 &p) const {
    vec3 shifted = p - center;

    return generic_eval<1, 1, 1, 1>(shifted, radii[0], radii[1], radii[2]);
}

std::pair<vec3, vec3> ellipsoid::bounding_box() const {
    return {center - radii, center + radii};
}

double x_cone::distance_along_line(const vec3 &p, const vec3 &dir) const {
    return generic_dist<-1, 1, 1, 0>(p - center, dir, a, b, c);
}

bool x_cone::point_inside(const vec3 &p) const {
    return generic_eval<-1, 1, 1, 0>(p - center, a, b, c);
}

std::pair<vec3, vec3> x_cone::bounding_box() const {
    // cone cannot be bounded
    return {{-constants::infinity, -constants::infinity, -constants::infinity},
            {constants::infinity, constants::infinity, constants::infinity}};
}

double y_cone::distance_along_line(const vec3 &p, const vec3 &dir) const {
    return generic_dist<1, -1, 1, 0>(p - center, dir, a, b, c);
}

bool y_cone::point_inside(const vec3 &p) const {
    return generic_eval<1, -1, 1, 0>(p - center, a, b, c);
}

std::pair<vec3, vec3> y_cone::bounding_box() const {
    // cone cannot be bounded
    return {{-constants::infinity, -constants::infinity, -constants::infinity},
            {constants::infinity, constants::infinity, constants::infinity}};
}

double z_cone::distance_along_line(const vec3 &p, const vec3 &dir) const {
    return generic_dist<1, 1, -1, 0>(p - center, dir, a, b, c);
}

bool z_cone::point_inside(const vec3 &p) const {
    return generic_eval<1, 1, -1, 0>(p - center, a, b, c);
}

std::pair<vec3, vec3> z_cone::bounding_box() const {
    // cone cannot be bounded
    return {{-constants::infinity, -constants::infinity, -constants::infinity},
            {constants::infinity, constants::infinity, constants::infinity}};
}

double x_cylinder::distance_along_line(const vec3 &p, const vec3 &dir) const {
    return generic_dist<0, 1, 1, 1>(p - center, dir, 1, a, b);
}

bool x_cylinder::point_inside(const vec3 &p) const {
    return generic_eval<0, 1, 1, 1>(p - center, 1, a, b);
}

std::pair<vec3, vec3> x_cylinder::bounding_box() const {
    return {{-constants::infinity, center.y() - a, center.z() - b},
            {constants::infinity, center.y() - a, center.z() - b}};
}

double y_cylinder::distance_along_line(const vec3 &p, const vec3 &dir) const {
    return generic_dist<1, 0, 1, 1>(p - center, dir, a, 1, b);
}

bool y_cylinder::point_inside(const vec3 &p) const {
    return generic_eval<1, 0, 1, 1>(p - center, a, 1, b);
}

std::pair<vec3, vec3> y_cylinder::bounding_box() const {
    return {{center.x() - a, -constants::infinity, center.z() - b},
            {center.x() + b, constants::infinity, center.z() + b}};
}

double z_cylinder::distance_along_line(const vec3 &p, const vec3 &dir) const {
    return generic_dist<1, 1, 0, 1>(p - center, dir, a, b, 1);
}

bool z_cylinder::point_inside(const vec3 &p) const {
    return generic_eval<1, 1, 0, 1>(p - center, a, b, 1);
}

std::pair<vec3, vec3> z_cylinder::bounding_box() const {
    return {{center.x() - a, center.y() - b, -constants::infinity},
            {center.x() + a, center.y() + b, constants::infinity}};
}

} // namespace projector