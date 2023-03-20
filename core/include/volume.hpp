#pragma once
#include <optional>
#include <utility>
#include <Eigen/Dense>
#include "path.hpp"

namespace projector {

using vec3 = Eigen::Vector3d;

template <typename T>
concept generic_volume =
requires(T t, vec3 vect, segment seg) {
    {t.bounding_box()} -> std::same_as<std::pair<vec3, vec3>>;
    {t.size()} -> std::same_as<vec3>;
    {t.map_sampling(vect)} -> std::same_as<vec3>;
    {t.intersection(seg)} -> std::same_as<std::optional<segment>>;
};

struct AA_box {
    vec3 min_point;
    vec3 max_point;

    std::pair<vec3, vec3> bounding_box() const;
    vec3 size() const;
    vec3 map_sampling(const vec3& point) const;
    std::optional<segment> intersection(const segment& seg) const;
};

struct AA_cylinder {
    vec3 start_point;
    vec3 axis;
    vec3 dimensions;

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
