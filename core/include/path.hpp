#pragma once
#include <vector>
#include <Eigen/Dense>

namespace projector {

using vec3 = Eigen::Vector3d;

struct segment {
    vec3 start;
    vec3 dir;
    double t_max;

    inline vec3 point_at(const double t) const {
        return start + dir * t;
    }

    inline vec3 end_point() const {
        return point_at(t_max);
    }

    inline double length() const {
        return (dir * t_max).norm();
    }
};

struct path {
    std::vector<segment> segments;

    segment& segment_at(const double t) const;
    vec3 point_at(const double t) const;
    double total_length() const;

};


} //namespace projector