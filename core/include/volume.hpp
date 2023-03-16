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
    virtual vec3 map_coordinates(vec3 point) const = 0;
    virtual std::optional<segment> intersection(const segment& previous) const = 0;

};

struct AA_box : generic_volume {
    vec3 min_point;
    vec3 max_point;
};

struct AA_cylinder : generic_volume {
    vec3 start_point;
    vec3 end_point;
    double radius;
};

struct AA_ellipsoid : generic_volume {
    vec3 center;
    vec3 radius;
};

bool bounding_box_check(const std::pair<vec3, vec3>& bb, const segment& line);

} //namespace projector
