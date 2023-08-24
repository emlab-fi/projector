#pragma once

#include <Eigen/Dense>
#include <utility>

namespace projector {

using vec3 = Eigen::Vector3d;

/// @brief Abstract interface class representing a quadric surface
///
/// This class acts as the base class for additional surface classes.
/// As such it has no implementation and it is fully abstract.
/// Any derived surfaces should not have any other public methods apart
/// from these virtual methods defined in this class.
struct surface {

    virtual ~surface() = default;

    /// Calculate distance from point to the surface along a line.
    ///
    /// @param p The point from which to calculate distance
    /// @param dir The vector defining the direction of the line, normalized
    ///
    /// @return Distance to the surface. Negative distance is inside the
    /// surface.
    ///
    virtual double distance_along_line(const vec3 &p,
                                       const vec3 &dir) const = 0;

    /// Determine whether a point is inside a surface.
    ///
    /// @param p The point to determine.
    /// @return True if point is inside the surface, otherwise false
    ///
    virtual bool point_inside(const vec3 &p) const = 0;

    /// Calculate the bounding box of the surface.
    ///
    /// @return pair of min and max points of the bounding box.
    ///
    virtual std::pair<vec3, vec3> bounding_box() const = 0;
};

/// A generic surface defined by a point and a normal vector
class plane : public surface {

    vec3 point;
    vec3 normal;

  public:
    plane(vec3 center, vec3 norm) : point(center), normal(norm) {}

    double distance_along_line(const vec3 &p, const vec3 &dir) const final;
    bool point_inside(const vec3 &p) const final;
    std::pair<vec3, vec3> bounding_box() const final;
};

/// Generic axis alligned ellipsoid, defined by a center point and 3 radii.
class ellipsoid : public surface {

    vec3 center;
    vec3 radii;

  public:
    ellipsoid(vec3 center, double a, double b, double c)
        : center(center), radii({a, b, c}) {}

    double distance_along_line(const vec3 &p, const vec3 &dir) const final;
    bool point_inside(const vec3 &p) const final;
    std::pair<vec3, vec3> bounding_box() const final;
};

/// X axis aligned elliptic cone, defined by center point and parameters a, b, c
class x_cone : public surface {

    vec3 center;
    double a;
    double b;
    double c;

  public:
    x_cone(vec3 center, double a, double b, double c)
        : center(center), a(a), b(b), c(c) {}

    double distance_along_line(const vec3 &p, const vec3 &dir) const final;
    bool point_inside(const vec3 &p) const final;
    std::pair<vec3, vec3> bounding_box() const final;
};

/// Y axis aligned elliptic cone, defined by center point and parameters a, b, c
class y_cone : public surface {

    vec3 center;
    double a;
    double b;
    double c;

  public:
    y_cone(vec3 center, double a, double b, double c)
        : center(center), a(a), b(b), c(c) {}

    double distance_along_line(const vec3 &p, const vec3 &dir) const final;
    bool point_inside(const vec3 &p) const final;
    std::pair<vec3, vec3> bounding_box() const final;
};

/// Z axis aligned elliptic cone, defined by center point and parameters a, b, c
class z_cone : public surface {

    vec3 center;
    double a;
    double b;
    double c;

  public:
    z_cone(vec3 center, double a, double b, double c)
        : center(center), a(a), b(b), c(c) {}

    double distance_along_line(const vec3 &p, const vec3 &dir) const final;
    bool point_inside(const vec3 &p) const final;
    std::pair<vec3, vec3> bounding_box() const final;
};

/// X axis aligned elliptic cylinder, defined by center point and 2 radii a, b
class x_cylinder : public surface {

    vec3 center;
    double a;
    double b;

  public:
    x_cylinder(vec3 center, double a, double b) : center(center), a(a), b(b) {}

    double distance_along_line(const vec3 &p, const vec3 &dir) const final;
    bool point_inside(const vec3 &p) const final;
    std::pair<vec3, vec3> bounding_box() const final;
};

/// Y axis aligned elliptic cylinder, defined by center point and 2 radii a, b
class y_cylinder : public surface {

    vec3 center;
    double a;
    double b;

  public:
    y_cylinder(vec3 center, double a, double b) : center(center), a(a), b(b) {}

    double distance_along_line(const vec3 &p, const vec3 &dir) const final;
    bool point_inside(const vec3 &p) const final;
    std::pair<vec3, vec3> bounding_box() const final;
};

/// Z axis aligned elliptic cylinder, defined by center point and 2 radii a, b
class z_cylinder : public surface {

    vec3 center;
    double a;
    double b;

  public:
    z_cylinder(vec3 center, double a, double b) : center(center), a(a), b(b) {}

    double distance_along_line(const vec3 &p, const vec3 &dir) const final;
    bool point_inside(const vec3 &p) const final;
    std::pair<vec3, vec3> bounding_box() const final;
};


} // namespace projector