#pragma once
#include <optional>
#include <utility>
#include <Eigen/Dense>
#include "path.hpp"

namespace projector {

using vec3 = Eigen::Vector3d;


struct AA_box {
    vec3 min_point;
    vec3 max_point;

    std::pair<vec3, vec3> bounding_box() const;
    vec3 size() const;
    vec3 map_sampling(const vec3& point) const;
    std::optional<segment> intersection(const segment& seg) const;
};

struct AA_cylinder{
    vec3 start_point;
    vec3 end_point;
    double radius;

    std::pair<vec3, vec3> bounding_box() const;
    vec3 size() const;
    vec3 map_sampling(const vec3& point) const;
    std::optional<segment> intersection(const segment& seg) const;
};

struct AA_ellipsoid {
    vec3 center;
    vec3 radius;

    std::pair<vec3, vec3> bounding_box() const;
    vec3 size() const;
    vec3 map_sampling(const vec3& point) const;
    std::optional<segment> intersection(const segment& seg) const;
};

bool bounding_box_check(const std::pair<vec3, vec3>& bb, const segment& line);

} //namespace projector
