#pragma once
#include <vector>
#include <cmath>
#include <optional>
#include <nlohmann/json.hpp>
#include <Eigen/Dense>

namespace geom {

using point = Eigen::Vector3d;

double dot(const point& a, const point& b);

double vec_size(const point& a);

point normalize(const point& a);

struct ray {
    point start;
    point dir;

    inline point point_at(double t) const {
        return start + t * dir;
    }
};

struct z_cylinder {
    point center;
    double height;
    double radius;
};

enum class area_shape {
    ellipse,
    rectangle
};

struct z_area_2d {
    area_shape shape;
    point center;
    double a;
    double b;
};

std::vector<point> sphere_fibonnaci(unsigned int samples);

std::optional<double> ray_intersects_z_cylinder(const ray& r, const z_cylinder& c);

std::optional<double> ray_intersects_z_area(const ray& r, const z_area_2d& a);

void from_json(const nlohmann::json& j, point& p);
void from_json(const nlohmann::json& j, z_area_2d& a);

} //namespace geom
