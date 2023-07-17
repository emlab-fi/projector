#pragma once

#include <variant>
#include <memory>
#include <Eigen/Dense>

namespace projector {

using vec3 = Eigen::Vector3d;


struct geom_primitive {
    enum class shape {
        plane, aa_box, aa_ellipsoid
    };

    shape type;
    vec3 param1;
    vec3 param2;

    bool point_is_inside(const vec3& point) const;

};

struct geometry;

struct operation {
    enum class type : std::size_t {
        nop = 0, join, intersect, cut
    };

    type op;
    std::unique_ptr<geometry> left;
    std::unique_ptr<geometry> right;

    bool point_is_inside(const vec3& point) const;
};


struct geometry {
    std::variant<geom_primitive, operation> definition;

    geometry() = default;

    geometry(geometry&& geom) : definition(std::move(geom.definition)) {}

    geometry(const geom_primitive& prim) : definition(prim) {}

    geometry(operation&& op) : definition(std::move(op)) {}

    bool point_is_inside(const vec3& point) const;

    vec3 sample_point(uint64_t& prng_state) const;

};

vec3 rotate_direction(vec3 dir, double mu, double phi);

} //namespace projector