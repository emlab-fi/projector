#include <cmath>
#include "volume.hpp"

namespace {

static constexpr uint8_t x_min_flag = 0b0000'0001;
static constexpr uint8_t x_max_flag = 0b0000'0010;
static constexpr uint8_t y_min_flag = 0b0000'0100;
static constexpr uint8_t y_max_flag = 0b0000'1000;
static constexpr uint8_t z_min_flag = 0b0001'0000;
static constexpr uint8_t z_max_flag = 0b0010'0000;

uint8_t bb_outcode(const std::pair<projector::vec3, projector::vec3>& bb, const projector::vec3& point) {

    uint8_t out_code = 0U;

    if (point[0] < bb.first[0]) {
        out_code |= x_min_flag;
    }
    if (point[0] > bb.second[0]) {
        out_code |= x_max_flag;
    }
    if (point[1] < bb.first[1]) {
        out_code |= y_min_flag;
    }
    if (point[1] > bb.second[1]) {
        out_code |= y_max_flag;
    }
    if (point[2] < bb.first[2]) {
        out_code |= z_min_flag;
    }
    if (point[2] > bb.second[2]) {
        out_code |= z_max_flag;
    }

    return out_code;
}

double intersect_bb(const std::pair<projector::vec3, projector::vec3>& bb, const uint8_t code, const projector::segment& seg) {
    double new_t = 0;

    if (code & x_min_flag) {
        new_t = ((bb.first - seg.start).dot(projector::vec3{1, 0, 0})) / bb.first.dot(seg.dir);
    }
    else if (code & x_max_flag) {
        new_t = ((bb.second - seg.start).dot(projector::vec3{1, 0, 0})) / bb.second.dot(seg.dir);
    }
    else if (code & y_min_flag) {
        new_t = ((bb.first - seg.start).dot(projector::vec3{0, 1, 0})) / bb.first.dot(seg.dir);
    }
    else if (code & y_max_flag) {
        new_t = ((bb.second - seg.start).dot(projector::vec3{0, 1, 0})) / bb.second.dot(seg.dir);
    }
    else if (code & z_min_flag) {
        new_t = ((bb.first - seg.start).dot(projector::vec3{0, 0, 1})) / bb.first.dot(seg.dir);
    }
    else { //z_max_flag
        new_t = ((bb.second - seg.start).dot(projector::vec3{0, 0, 1})) / bb.second.dot(seg.dir);
    }

    return new_t;
}

} // namespace

namespace projector {

//
// AA_box
//

std::pair<vec3, vec3> AA_box::bounding_box() const {
    return {min_point, max_point};
}

vec3 AA_box::size() const {
    return max_point - min_point;
}

vec3 AA_box::map_sampling(const vec3& point) const {
    return min_point + (size().asDiagonal() * point);
}

std::optional<segment> AA_box::intersection(const segment& seg) const {
    // Currently uses Cohen-Sutherland, which can get quite slow
    segment new_segment = seg;

    bool accept = false;

    // at max 6 intersections to solve
    for (int i = 0; i < 6; ++i) {

        uint8_t start_outcode = bb_outcode({min_point, max_point}, new_segment.start);
        uint8_t end_outcode = bb_outcode({min_point, max_point}, new_segment.end_point());

        if (!(start_outcode | end_outcode)) {
            accept = true;
            break;
        }

        if (start_outcode & end_outcode) {
            break;
        }

        uint8_t current_code = start_outcode > end_outcode ? start_outcode : end_outcode;

        double new_t = intersect_bb({min_point, max_point}, current_code, new_segment);

        if (current_code == start_outcode) {
            new_segment.start = new_segment.point_at(new_t);
            new_segment.t_max -= new_t;
        } else {
            new_segment.t_max = new_t;
        }
    }

    if (accept) {
        return new_segment;
    }
    return {};
}

//
// AA_cylinder
//

std::pair<vec3, vec3> AA_cylinder::bounding_box() const {
}

vec3 AA_cylinder::size() const {
}

vec3 AA_cylinder::map_sampling(const vec3& point) const {

}

std::optional<segment> AA_cylinder::intersection(const segment& seg) const {
    return {};
}

//
// AA_ellipsoid
//

std::pair<vec3, vec3> AA_ellipsoid::bounding_box() const {
    vec3 min_point = center - radius;
    vec3 max_point = center - radius;
    return {min_point, max_point};
}

vec3 AA_ellipsoid::size() const {
    return radius * 2;
}


vec3 AA_ellipsoid::map_sampling(const  vec3& point) const {
    // TODO: change this mapping to keep the uniformity
    // take the input as normal vector and normalize it if over length of 1
    // this does not keep the uniformity -> squishes corner points more

    vec3 sphere_point = (point - vec3{0.5, 0.5, 0.5}) * 2.0;

    double length = sphere_point.norm();

    if (length > 1) {
        sphere_point /= length;
    }

    // scale and translate to volume
    return center + (size().asDiagonal() * sphere_point);
}

std::optional<segment> AA_ellipsoid::intersection(const segment& seg) const {
    return {};
}

//
// Helper functions
//

bool bounding_box_check(const std::pair<vec3, vec3>& bb, const segment& line) {
    //TODO: make this actually work
    return true;
}

} //namespace projector