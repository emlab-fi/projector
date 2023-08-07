#include "geometry.hpp"

#include "constants.hpp"
#include "random_numbers.hpp"

#include <cmath>

namespace {


using pvec3 = projector::vec3;

pvec3 box_random_sample(const pvec3 &min, const pvec3 &max,
                        uint64_t &prng_state) {
    pvec3 output = {0.0, 0.0, 0.0};

    for (std::size_t i = 0; i < 3; ++i) {
        output[i] =
            min[i] + projector::prng_double(prng_state) * (max[i] - min[i]);
    }

    return output;
}

constexpr double signum(const double d) {
    if (d > 0.0)
        return 1;
    if (d == 0.0)
        return 0;
    return -1;
}

} // namespace

namespace projector {

void geometry::add_surface(std::unique_ptr<surface> surface, csg_operation op) {
    surfaces.push_back(std::move(surface));
    ops.push_back(op);
}

void geometry::add_surface(geometry geom, csg_operation op) {
    surfaces.push_back(std::move(geom));
    ops.push_back(op);
}

double geometry::nearest_surface_distance(const vec3 &point,
                                          const vec3 &dir) const {

    auto visitor = [&point, &dir](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<std::unique_ptr<surface>, T>) {
            return arg->distance_along_line(point, dir);
        } else if constexpr (std::is_same_v<T, geometry>) {
            return arg.nearest_surface_distance(point, dir);
        }
        return constants::infinity;
    };

    // this currently produces false positives when dealing with intersections
    // and substractions, we need to solve that maybe?
    double distance = constants::infinity;

    for (std::size_t i = 0; i < surfaces.size(); ++i) {
        double dist_to_surface = std::visit(visitor, surfaces[i]);
        distance = std::min(distance, dist_to_surface);
    }

    return distance;
}

bool geometry::point_inside(const vec3 &point) const {

    auto visitor = [&point](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<std::unique_ptr<surface>, T>) {
            return arg->point_inside(point);
        } else if constexpr (std::is_same_v<T, geometry>) {
            return arg.point_inside(point);
        }
        return false;
    };

    bool inside = true;

    for (std::size_t i = 0; i < surfaces.size(); ++i) {

        bool inside_surface = std::visit(visitor, surfaces[i]);

        switch (ops[i]) {
        case csg_operation::no_op:
            break;
        case csg_operation::join:
            inside = inside || inside_surface;
            break;
        case csg_operation::intersect:
            inside = inside && inside_surface;
            break;
        case csg_operation::substract:
            inside = inside && !inside_surface;
            break;
        default:
            inside = false;
            break;
        }
    }

    return inside;
}

vec3 geometry::sample_point(uint64_t &prng_state) const {

    // basic rejection sampling method:
    // - sample the bounding box
    // - check whether point is inside our geometry
    while (true) {
        vec3 sampled_point = box_random_sample(bounding_box.first,
                                               bounding_box.second, prng_state);

        if (point_inside(sampled_point)) {
            return sampled_point;
        }
    }

    return {0.0, 0.0, 0.0};
}

void geometry::update_bounding_box(const vec3 &min, const vec3 &max) {

    vec3 current_min = min;
    vec3 current_max = max;

    auto visitor = [](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<std::unique_ptr<surface>, T>) {
            return arg->bounding_box();
        } else if constexpr (std::is_same_v<T, geometry>) {
            return arg.bounding_box;
        }
    };

    for (std::size_t i = 0; i < surfaces.size(); ++i) {

        auto [s_min, s_max] = std::visit(visitor, surfaces[i]);

        switch (ops[i]) {
            case csg_operation::no_op:
                break;
            case csg_operation::join:
                current_min = current_min.cwiseMin(s_min);
                current_max = current_max.cwiseMax(s_max);
                break;
            case csg_operation::intersect:
                current_min = current_min.cwiseMax(s_min);
                current_max = current_max.cwiseMin(s_max);
                break;
            case csg_operation::substract:
                break;
            default:
                break;
        }

    }

    bounding_box = {current_min, current_max};

}

vec3 rotate_direction(vec3 dir, double mu, double phi) {
    // done according to PENELOPE 2018 docs and OpenMC source code
    // should be simple axis-angle rotation where axis is the original direction
    // mu is the cosine of angle deflected from the axis
    // phi is angle in radians to rotate around the axis

    // precalculate
    double sin_phi = std::sin(phi);
    double cos_phi = std::cos(phi);
    double sin_mu = std::sqrt(1 - mu * mu); // get absolute sine from cosine
    double a = std::sqrt(1 - dir[2] * dir[2]);

    double u, v, w;
    // need to take care of special case when a is close to zero, as then it is
    // parallel to z-axis we can use simpler functions when it is
    // TODO: check whether this is enough or we need OpenMC approach
    if (a > 1e-10) {
        u = dir[0] * mu +
            (sin_mu / a) * (dir[0] * dir[2] * cos_phi - dir[1] * sin_phi);
        v = dir[1] * mu +
            (sin_mu / a) * (dir[1] * dir[2] * cos_phi + dir[0] * sin_phi);
        w = dir[2] * mu - a * sin_mu * cos_phi;
    } else {
        double sign = signum(dir[2]);
        u = sign * sin_mu * cos_phi;
        v = sign * sin_mu * sin_phi;
        w = sign * mu;
    }

    return {u, v, w};
}

vec3 random_unit_vector(uint64_t &prng_state) {
    double u = projector::prng_double(prng_state);
    double v = projector::prng_double(prng_state);

    double theta = 2.0 * projector::constants::pi * u;
    double phi = std::acos(2.0 * v - 1);

    return {std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi),
            std::cos(theta)};
}

} // namespace projector