#include <cmath>
#include "volume.hpp"

namespace projector {

//
//AA_box
//

std::pair<vec3, vec3> AA_box::bounding_box() const {
    return {min_point, max_point};
}

vec3 AA_box::size() const {
    return max_point - min_point;
}

vec3 AA_box::map_sampling(const vec3& point) const {
    vec3 scaling_factors = size();

    Eigen::Matrix3d scaling_matrix {
        {scaling_factors[0], 0, 0},
        {0, scaling_factors[1], 0},
        {0, 0, scaling_factors[2]}
    };

    return min_point + (scaling_matrix * point);
}

std::optional<segment> AA_box::intersection(const segment& previous) const {
    return {};
}

//
//AA_cylinder
//

std::pair<vec3, vec3> AA_cylinder::bounding_box() const {
}

vec3 AA_cylinder::size() const {

}

vec3 AA_cylinder::map_sampling(const vec3& point) const {

}

std::optional<segment> AA_cylinder::intersection(const segment& previous) const {
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

    double length2 = sphere_point[0] * sphere_point[0]
                   + sphere_point[1] * sphere_point[1]
                   + sphere_point[2] * sphere_point[2];

    if (length2 > 1) {
        sphere_point /= std::sqrt(length2);
    }

    // scale and translate to volume
    vec3 scaling_factors = size();

    Eigen::Matrix3d scaling_matrix {
        {scaling_factors[0], 0, 0},
        {0, scaling_factors[1], 0},
        {0, 0, scaling_factors[2]}
    };

    return center + (scaling_matrix * sphere_point);
}

std::optional<segment> AA_ellipsoid::intersection(const segment& previous) const {
    return {};
}

} //namespace projector