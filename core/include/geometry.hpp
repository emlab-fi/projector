#pragma once

#include "surface.hpp"

#include <Eigen/Dense>
#include <memory>
#include <variant>

namespace projector {

using vec3 = Eigen::Vector3d;

enum class csg_operation {
    no_op,
    join,
    intersect,
    substract
};

class geometry {

    using surface_item = std::pair<csg_operation, std::variant<geometry, surface>>;

    std::vector<surface_item> surfaces;

    std::pair<vec3, vec3> bounding_box;

public:

    void add_element(std::variant<geometry, surface>, csg_operation op);

    double distance_along_line(const vec3 &point, const vec3& dir) const;

    bool point_is_inside(const vec3 &point) const;

    vec3 sample_point(uint64_t &prng_state) const;

    void update_bounding_box(vec3 min, vec3 max);

};

vec3 rotate_direction(vec3 dir, double mu, double phi);

vec3 random_unit_vector(uint64_t &prng_state);

} // namespace projector