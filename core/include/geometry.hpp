#pragma once

#include "surface.hpp"

#include <Eigen/Dense>
#include <memory>
#include <variant>

namespace projector {

using vec3 = Eigen::Vector3d;

using coord3 = Eigen::Vector3i;

enum class csg_operation { no_op, join, intersect, substract };

struct bounding_box {
    vec3 min;
    vec3 max;

    bool point_inside(const vec3 &point) const;

    bool point_inside_2d(const vec3& point, std::size_t index1, std::size_t index2) const;

    bool line_intersects(const vec3& point, const vec3& dir) const;

    bool segment_intersect(const vec3& start, const vec3& end) const;

    double distance_along_line(const vec3 &point, const vec3 &dir) const;

    vec3 random_sample(uint64_t &prng_state) const;
};

class geometry {

    std::vector<std::variant<geometry, std::unique_ptr<surface>>> surfaces;
    std::vector<csg_operation> ops;

public:

    bounding_box bb;

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