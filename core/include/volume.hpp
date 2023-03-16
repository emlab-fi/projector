#pragma once
#include <optional>
#include <utility>
#include <Eigen/Dense>
#include "path.hpp"

namespace projector {

using vec3 = Eigen::Vector3d;


struct generic_volume {
    virtual ~generic_volume() = default;

    virtual std::pair<vec3, vec3> bounding_box() const = 0;
    virtual vec3 size() const = 0;
    virtual vec3 map_sampling(const vec3& point) const = 0;
    virtual std::optional<segment> intersection(const segment& previous) const = 0;

};

struct AA_box final : generic_volume {
    vec3 min_point;
    vec3 max_point;

    std::pair<vec3, vec3> bounding_box() const;
    vec3 size() const;
    vec3 map_sampling(const vec3& point) const;
    std::optional<segment> intersection(const segment& previous) const;
};

struct AA_cylinder final : generic_volume {
    vec3 start_point;
    vec3 end_point;
    double radius;

    std::pair<vec3, vec3> bounding_box() const;
    vec3 size() const;
    vec3 map_sampling(const vec3& point) const;
    std::optional<segment> intersection(const segment& previous) const;
};

struct AA_ellipsoid final : generic_volume {
    vec3 center;
    vec3 radius;

    std::pair<vec3, vec3> bounding_box() const;
    vec3 size() const;
    vec3 map_sampling(const vec3& point) const;
    std::optional<segment> intersection(const segment& previous) const;
};

bool bounding_box_check(const std::pair<vec3, vec3>& bb, const segment& line);

} //namespace projector
