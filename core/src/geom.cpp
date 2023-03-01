#include <vector>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include "geom.hpp"
#include <iostream>

namespace {

constexpr double EPSILON = 0.001;

constexpr bool within_epsilon(const double test, const double target) {
    return abs(test-target) <= EPSILON;
}

constexpr double clamp(const double in, const double low, const double high) {
    if (in < low) {
        return low;
    }
    if (in > high) {
        return high;
    }
    return in;
}

}

namespace geom {

const double& point::operator[](int index) const {
    switch(index) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
    }
    return x;
};

double& point::operator[](int index) {
    switch(index) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
    }
    return x;
};

point operator+(const point& a, const point& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

point operator-(const point& a, const point& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

point operator*(const double& a, const point& b) {
    return {a*b.x, a*b.y, a*b.z};
}

point operator*(const point& a, const double& b) {
    return b*a;
}

point operator/(const point& a, const double& b) {
    return {a.x/b, a.y/b, a.z/b};
}

double dot(const point& a, const point& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double vec_size(const point& a) {
    return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

point normalize(const point& a) {
    return a/vec_size(a);
}

std::vector<point> sphere_fibonnaci(unsigned int samples) {
    std::vector<point> output = {};

    double phi = std::numbers::pi * (3.0 - std::sqrt(5.0));

    for (unsigned int i = 0; i < samples; i++) {
        double y = 1.0 - (i / (static_cast<double>(samples) - 1)) * 2.0;
        double radius = std::sqrt(1.0 - y*y);
        double theta = i * phi;
        double x = std::cos(theta) * radius;
        double z = std::sin(theta) * radius;

        output.push_back({x, y, z});
    }
    return output;
}

std::optional<double> ray_intersects_z_cylinder(const ray& r, const z_cylinder& c) {
    double t_max = ((c.center.z + c.height * 0.5) - r.start.z) / r.dir.z;
    double t_min = ((c.center.z - c.height * 0.5) - r.start.z) / r.dir.z;

    // rays are unidirectional
    if (t_max < 0 && t_min < 0) {
        return {};
    }

    //flaten into 2D problem
    point center_2d = {c.center.x, c.center.y, 0};

    ray r_2d = {
        .start = {r.start.x, r.start.y, 0},
        .dir = {r.dir.x, r.dir.y, 0}
    };

    //solve for t
    double A = r_2d.dir.x * r_2d.dir.x + r_2d.dir.y * r_2d.dir.y;
    double B = 2.0 * (r_2d.dir.x * (r_2d.start.x - center_2d.x) + r_2d.dir.y * (r_2d.start.y - center_2d.y));
    double C_x = r_2d.start.x - center_2d.x;
    double C_y = r_2d.start.y - center_2d.y;
    double C = C_x * C_x + C_y * C_y - c.radius * c.radius;

    double D = (B * B) - (4.0 * A * C);

    if (A <= 0.000001 || D < 0.0) {
        return {};
    }

    if (D == 0.0) {
        return 0.0;
    }

    double t_1 = (-B + std::sqrt(D)) / (2.0 * A);
    double t_2 = (-B - std::sqrt(D)) / (2.0 * A);

    if ((t_1 < t_min && t_2 < t_min) || (t_1 > t_max && t_2 > t_max)) {
        return {};
    }

    if ((t_1 < 0 && t_2 > 0) || (t_1 > 0 && t_2 < 0)) {
        t_1 = clamp(t_1, t_min, t_max);
        t_2 = clamp(t_2, t_min, t_max);
    }

    point a = r.point_at(t_1);
    point b = r.point_at(t_2);

    return vec_size(a - b);
}

std::optional<double> ray_intersects_z_area(const ray& r, const z_area_2d& target) {

    double t = (target.center.z - r.start.z) / r.dir.z;

    //rays are single direction only
    if (t < 0.0) {
        return {};
    }

    point in_plane = r.point_at(t);

    if (target.shape == area_shape::ellipse) {
        double x_temp = (in_plane.x - target.center.x) * (in_plane.x - target.center.x);
        double y_temp = (in_plane.y - target.center.y) * (in_plane.y - target.center.y);
        double a_half = target.a / 2.0;
        double b_half = target.b / 2.0;
        double result = x_temp / (a_half * a_half) + y_temp / (b_half * b_half);

        if (result <= 1.0) {
            return t;
        }
        return {};
    }

    else if (target.shape == area_shape::rectangle) {
        point shifted = in_plane - target.center;
        double half_a = target.a/2.0;
        double half_b = target.b/2.0;

        if (shifted.x >= half_a || shifted.x <= -half_a) {
            return {};
        }
        if (shifted.y >= half_b || shifted.y <= -half_b) {
            return {};
        }
        return t;
    }

    return {};
}

void from_json(const nlohmann::json& j, point& p) {
    if (!j.is_array()) {
        throw std::invalid_argument("JSON point is not an array.");
    }

    j.at(0).get_to(p.x);
    j.at(1).get_to(p.y);
    j.at(2).get_to(p.z);
}

void from_json(const nlohmann::json& j, z_area_2d& a) {
    if (!j.is_object()) {
        throw std::invalid_argument("JSON area is not an object.");
    }
    if (!j.at("dimensions").is_array()) {
        throw std::invalid_argument("JSON area dimensions are not an array.");
    }

    if (j.at("type") == "ellipse") a.shape = area_shape::ellipse;
    else if (j.at("type") == "rectangle") a.shape = area_shape::rectangle;
    else throw std::invalid_argument("JSON area type is not supported.");

    j.at("center").get_to(a.center);
    j.at("dimensions").at(0).get_to(a.a);
    j.at("dimensions").at(1).get_to(a.b);
}

} //namespace geom