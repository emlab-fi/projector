# Geometry

[TOC]

Projector uses similar approach to geometry as other particle transport simulators, a combination of [computational solid geometry](https://en.wikipedia.org/wiki/Constructive_solid_geometry) and [quadric](https://en.wikipedia.org/wiki/Quadric) surfaces that define half-spaces.
Quadrics are generic definitions of surfaces in 3D that allow algorithmic calculation of intersection between particle and the surface. They are also simple to define primitive shapes with (and some primitives are directly a quadric surface, ie. a sphere).
CSG allows us to build complex geometries from primitives and it is both simple to grasp conceptually and to run algorithmic calculations on.

All the geometry in projector uses cartesian coordinate system, although some calculation steps might temporarily change to polar or other coordinate system.

## Computational solid geometry

CSG works by composing primitives into a binary tree structure using boolean operators. Project supports 3 basic operators - union, intersection and substraction. These operations are quite self-descriptive. There are two notations used interchangeably in Projector.

Operation | Set notation | Algebraic notation |
---------:|:------------:|:------------------:|
Union | \f$ \cup \f$ | + |
Intersect | \f$ \cap \f$ | * |
Substract | - ||


## Supported surfaces

### Generic plane

A generic 2D plane. When the normal vector is axis aligned, the plane is also axis aligned and the plane can be bounded in one direction.
Has 2 parameters, point `P` ("center") on the plane and the normal vector `N`.
The surface equation is as follows:

\f[
    P = (x_0, y_0, z_0) \\
    N = (a, b, c) \\

    a(x - x_0) + b(y - y_0) + c(z - z_0) = 0
\f]

As is common in computer graphics, the normal vector points "out" of the volume, so any point on the oppossite side is "inside" the plane when using the plane to construct complex geometry.

### Ellipsoid

Ellipsoid is a the generalization of a sphere, with different radii for each axis. It is defined by 4 parameters, the center `C` and the radii for each axis `a, b, c`. The surface equation is following:

\f[
    C = (x_0, y_0, z_0) \\

    \frac{(x - x_0)^2}{a^2} + \frac{(y - y_0)^2}{b^2} + \frac{(z - z_0)^2}{c^2} - 1 = 0
\f]

### Elliptic cone

There are 3 variants of the cone, each for one axis. Following equations are for z-cone. The equations for other axis variants differ in the sign of the fractions. The cone apex is located at the point `C`.

\f[
    C = (x_0, y_0, z_0) \\

    \frac{(x - x_0)^2}{a^2} + \frac{(y - y_0)^2}{b^2} - \frac{(z - z_0)^2}{c^2} = 0
\f]

### Elliptic cylinder

The cylinder is infinite and axis aligned. Following equations are for a z-cylinder. Other axis variants differ in the coordinates used to calculate the surface (x-axis uses y and z and vice versa). The cylinder is centered around point `C`.

\f[
    C = (x_0, y_0, z_0) \\

    \frac{(x - x_0)^2}{a^2} + \frac{(y - y_0)^2}{b^2} - 1 = 0
\f]


## Helper surfaces

Projector provides several helper surfaces for creating common geometric constructs.
They are internally converted to the typical surface representation.

### Box

This is a simple axis-aligned box, consisting of 6 surfaces.
It is defined by 2 parameters, the min and max coordinates:

\f[ (x_{min}, y_{min}, z_{min}), (x_{max}, y_{max}, z_{max}) \f]

### Capped cylinder

The cylinder has 1 more parameter `h`, which defines two planes along the cylinder axis, perpendicular to it. The planes are distanced `h/2` from the center point symmetrically.

### Capped cone

There are two additional parameters, `d1` and `d2`. They define the location of two planes. The plane defined by `d1` is this distance away from the center point of the cone and it's normal points to the negative direction. The plane defined by `d2` is similar, but the normal points to the positive direction.

## Bounding box calculation

We often need a bounding box for the geometry for various calculations, most notably sampling volumes and for acceleration of finding closest surfaces.
For some geometries, it can be easily computed - ellipsoids, cylinders, cuboids, capped cone.
Generic surfaces on the other hand usually can't be bounded, as they extend to infinity in one or more directions.
Multiple combined surfaces can be bounded, although some solutions are hard to compute algorithmically (ie. intersection of two generic planes).
Projector allows the user to define the bounding box manually (prefered) or it can do simple approximation of the bounding box (not tight) automatically when the bounding box is not supplied.

Some surfaces have easily computable bounds, which means we can construct a bounding box with atleast one axis that is not infinite.
Those surfaces are: plane parallel to axis, ellipsoid, elliptic cylinder, elliptic cone.
By traversing the tree of the geometry, calculating the bounding boxes of the elements and combining them according to following rules, we can calculate the approximation of the bounding box of the geometry. By also combining this bounding box with the bounds of the simulation, we get a bounding box that is always finite and in worst case spans the whole simulation environment.

Operation | min bounding box rule | max bounding box rule
------------------|---------------|----------------------
\f$ A \cup B \f$  | \f$ \min (A, B) \f$ | \f$ \max (A, B) \f$
\f$ A \cap B \f$  | \f$ \max (A, B) \f$ | \f$ \min (A, B) \f$
\f$ A - B \f$  |  \f$ A \f$


## Sampling random points from a CSG tree

Currently the simulator takes a simple approach to sampling random poitns from a CSG tree:

1. Sample random point from bounding box of the geometry
2. Check whether the point is in the geometry
3. Sample again or return the sampled point

## Distance to collision of a particle and a surface

For geometry purposes, particle is defined by it's current position \f$p\f$ and direction vector \f$d\f$ (unit vector).
If we take a generic surface defined by function \f$ F(x) = 0 \f$, we can define the intersection point of the particle and surface: \f$F(p + t * d) = 0\f$. By solving for parameter \f$t\f$, we get the distance. As the direction vector is a unit vector, this parameter is the distance to the collision.

## Determining distance to closest surface of a travelling particle

TODO: determine the algorithm for this.
Currently we just find the surface with smallest distance, which is not true when working with CSG geometries!
