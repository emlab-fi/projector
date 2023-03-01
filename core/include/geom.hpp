#pragma once
#include <vector>
#include <cmath>
#include <optional>
#include <nlohmann/json.hpp>

namespace geom {

struct point {
    double x;
    double y;
    double z;

    const double& operator[](int index) const;
    double& operator[](int index);
};

point operator+(const point& a, const point& b);

point operator-(const point& a, const point& b);

point operator*(const double& a, const point& b);

point operator*(const point& a, const double& b);

point operator/(const point& a, const double& b);

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
