# Geometry



## Computational solid geometry


## Supported quadric surfaces

### Generic plane

### Ellipsoid

### Elliptic cone

### Elliptic cylinder


## Helper geometries

Projector provides several helper geometries for creating common geometric constructs.
They are internally converted to the typical surface representation.

### Rectangular cuboid

This is a simple axis-aligned box, consisting of 6 surfaces.
It is defined by 6 parameters:

\f[ x_{min}, y_{min}, z_{min}, x_{max}, y_{max}, z_{max} \f]

### Capped cylinder

### Capped cone


## Bounding box calculation

We often need a bounding box for the geometry for various calculations, most notably sampling volumes and for acceleration of finding closest surfaces.
For some geometries, it can be easily computed - ellipsoids, cylinders, cuboids, capped cone.
Generic surfaces on the other hand usually can't be bounded, as they extend to infinity in one or more directions.
Multiple combined surfaces can be bounded, although some solutions are hard to compute algorithmically (ie. intersection of two generic planes).
Projector allows the user to define the bounding box manually (prefered) or it can do simple approximation of the bounding box (not tight) automatically when the bounding box is not supplied.

Some surfaces have easily computable bounds, which means we can construct a bounding box with atleast one axis that is not infinite.
Those surfaces are: axis aligned plane, ellipsoid, elliptic cylinder, elliptic cone.
By traversing the tree of the geometry, calculating the bounding boxes of the elements and combining them according to following rules, we can calculate the approximation of the bounding box of the geometry. By also combining this bounding box with the bounds of the simulation, we get a bounding box that is always finite and in worst case spans the whole simulation environment.

Boolean operation | bounding box rule
------------------|------------------
\f$ A \cup B \f$  | \f$ \max (A, B) \f$
\f$ A \cap B \f$  | \f$ \min (A, B) \f$
\f$ A - B \f$  | \f$ A \f$


## Sampling random points from a CSG tree

Currently the simulator takes a simple approach to sampling random poitns from a CSG tree:

1. Sample random point from bounding box of the geometry
2. Check whether the point is in the geometry
3. Sample again or return the sampled point