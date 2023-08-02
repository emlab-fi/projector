#include "surface.hpp"

#include "constants.hpp"

#include <cmath>

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
        if (point.z() < 0.0) {
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

    // the following math comes from expanding the surface formula from docs
    // and rearanging it so that we have a quadratic equation to solve distance
    // TODO: really should be in docs

    double a = std::pow(radii.y() * radii.z() * dir.x(), 2) +
               std::pow(radii.x() * radii.z() * dir.y(), 2) +
               std::pow(radii.x() * radii.y() * dir.z(), 2);

    double b = 2 * std::pow(radii.y() * radii.z(), 2) * shifted.x() * dir.x() +
               2 * std::pow(radii.x() * radii.z(), 2) * shifted.y() * dir.y() +
               2 * std::pow(radii.x() * radii.y(), 2) * shifted.z() * dir.z();

    double c = std::pow(radii.y() * radii.z() * shifted.x(), 2) +
               std::pow(radii.x() * radii.z() * shifted.y(), 2) +
               std::pow(radii.x() * radii.y() * shifted.z(), 2) -
               std::pow(radii.x() * radii.y() * radii.z(), 2);

    // we now have a*t^2 + b*t + c = 0, solve it via discriminant
    double discriminant = b * b - 4 * a * c;

    // if discriminant is negative, we have only complex roots and the line
    // doesn't hit the ellipsoid
    if (discriminant < 0.0) {
        return constants::infinity;
    }

    double dist1 = (-b + std::sqrt(discriminant)) / (2 * a);
    double dist2 = (-b - std::sqrt(discriminant)) / (2 * a);

    // if both solutions are positive, return the smaller one
    if (dist1 > 0.0 && dist2 > 0.0) {
        return std::min(dist1, dist2);
    }

    // particle is going away from the ellipsoid, return the greater value
    if (dist1 < 0.0 && dist2 < 0.0) {
        return std::max(dist1, dist2);
    }

    // we have one positive and one negative solution, return the positive
    return std::max(dist1, dist2);
}

bool ellipsoid::point_inside(const vec3 &p) const {
    vec3 shifted = p - center;
    double a = (shifted.x() * shifted.x()) / (radii.x() * radii.x());
    double b = (shifted.y() * shifted.y()) / (radii.y() * radii.y());
    double c = (shifted.z() * shifted.z()) / (radii.z() * radii.z());
    return (a + b + c) < 1.0;
}

std::pair<vec3, vec3> ellipsoid::bounding_box() const {
    return {-radii, radii};
}

} // namespace projector