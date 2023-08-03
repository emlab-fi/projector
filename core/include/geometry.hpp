#pragma once

#include "surface.hpp"

#include <Eigen/Dense>
#include <memory>
#include <variant>

namespace projector {

using vec3 = Eigen::Vector3d;

enum class csg_operation { no_op, join, intersect, substract };

class geometry {

    std::vector<std::variant<geometry, std::unique_ptr<surface>>> surfaces;
    std::vector<csg_operation> ops;

    std::pair<vec3, vec3> bounding_box;

public:

    void add_surface(std::unique_ptr<surface> surface, csg_operation op);

    void add_surface(geometry geom, csg_operation op);

    double nearest_surface_distance(const vec3 &point, const vec3 &dir) const;

    bool point_inside(const vec3 &point) const;

    vec3 sample_point(uint64_t &prng_state) const;

    void update_bounding_box(const vec3 &min, const vec3 &max);
};

vec3 rotate_direction(vec3 dir, double mu, double phi);

vec3 random_unit_vector(uint64_t &prng_state);

} // namespace projector