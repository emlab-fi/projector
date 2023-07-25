#include "geometry.hpp"
#include "random_numbers.hpp"
#include "constants.hpp"
#include <cmath>

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

constexpr double signum(const double d) {
    if (d > 0.0) return 1;
    if (d == 0.0) return 0;
    return -1;
}

pvec3 plane_random_sample( const pvec3& plane_p, const pvec3& normal, uint64_t& prng_state) {
    // very hacky way, sample from large sphere around the plane
    // and check whether it is "in" the plane
    while (true) {
        pvec3 output = ellipsoid_random_sample(plane_p, {100.0, 100.0, 100.0}, prng_state);
        if (point_inside_plane(output, plane_p, normal)) {
            return output;
        }
    }
}

pvec3 box_random_sample(const pvec3& min, const pvec3& max, uint64_t& prng_state) {
    pvec3 output = {0.0, 0.0, 0.0};

    for (std::size_t i = 0; i < 3; ++i) {
        output[i] = min[i] + projector::prng_double(prng_state) * (max[i] - min[i]);
    }

    return output;
}

pvec3 ellipsoid_random_sample(const pvec3& center, const pvec3& radii, uint64_t& prng_state) {
    double u = projector::prng_double(prng_state);
    double v = projector::prng_double(prng_state);

    double theta = 2.0 * projector::constants::pi * u;
    double phi = std::acos(2.0 * v - 1);
    double r = std::cbrt(projector::prng_double(prng_state));
    double sin_theta = std::sin(theta);
    double cos_theta = std::cos(theta);
    double sin_phi = std::sin(phi);
    double cos_phi = std::cos(phi);

    pvec3 output = {
        radii[0] * r * sin_phi * cos_theta,
        radii[1] * r * sin_phi * sin_theta,
        radii[2] * r * cos_phi
    };

    return output * center;
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


vec3 geom_primitive::sample_point(uint64_t& prng_state) const {
    switch (type) {
        case shape::plane:
            return plane_random_sample(param1, param2, prng_state);
        case shape::aa_box:
            return box_random_sample(param1, param2, prng_state);
        case shape::aa_ellipsoid:
            return ellipsoid_random_sample(param1, param2, prng_state);
        default:
            return vec3::Zero();
    }
    return vec3::Zero();
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


vec3 operation::sample_point(uint64_t& prng_state) const {

    if (op == type::join || op == type::intersect) {
        double sample = prng_double(prng_state);
        if (sample < 0.5) {
            return left->sample_point(prng_state);
        }
        return right->sample_point(prng_state);
    }

    //we dont need to sample right operand as we cut it out
    else if (op == type::cut) {
        return left->sample_point(prng_state);
    }
    return vec3::Zero();
}



bool geometry::point_is_inside(const vec3& point) const {
    return std::visit([&point](auto&& arg) {

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, geom_primitive>) {
            return arg.point_is_inside(point);
        } else if constexpr (std::is_same_v<T, operation>) {
            return arg.point_is_inside(point);
        }

        return false;
    }, definition);
}

vec3 geometry::sample_point(uint64_t& prng_state) const {

    // basic rejection sampling method:
    // - descend until a primitive, sample point in primitive
    // - check whether point is inside our geometry (due to intersections and such)
    // this can be quite long, as a small intersection will take long time to sample
    while (true) {
        vec3 sampled_point = std::visit([&prng_state](auto&& arg) {

            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, operation>) {
                return arg.sample_point(prng_state);
            } else if constexpr (std::is_same_v<T, geom_primitive>) {
                return arg.sample_point(prng_state);
            }

        }, definition);

        if (point_is_inside(sampled_point)) {
            return sampled_point;
        }
    }

    return {0.0, 0.0, 0.0};
}


vec3 rotate_direction(vec3 dir, double mu, double phi) {
    // done according to PENELOPE 2018 docs and OpenMC source code
    // should be simple axis-angle rotation where axis is the original direction
    // mu is the cosine of angle deflected from the axis
    // phi is angle in radians to rotate around the axis

    //precalculate
    double sin_phi = std::sin(phi);
    double cos_phi = std::cos(phi);
    double sin_mu = std::sqrt(1 - mu * mu); //get absolute sine from cosine
    double a = std::sqrt(1 - dir[2] * dir[2]);

    double u, v, w;
    // need to take care of special case when a is close to zero, as then it is parallel to z-axis
    // we can use simpler functions when it is
    // TODO: check whether this is enough or we need OpenMC approach
    if (a > 1e-10) {
        u = dir[0] * mu + (sin_mu / a) * (dir[0] * dir[2] * cos_phi - dir[1] * sin_phi);
        v = dir[1] * mu + (sin_mu / a) * (dir[1] * dir[2] * cos_phi + dir[0] * sin_phi);
        w = dir[2] * mu - a * sin_mu * cos_phi;
    } else {
        double sign = signum(dir[2]);
        u = sign * sin_mu * cos_phi;
        v = sign * sin_mu * sin_phi;
        w = sign * mu;
    }

    return {u, v, w};
}

}