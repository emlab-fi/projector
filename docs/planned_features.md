- axis aligned boxes for geometry
- also axis aligned cylinders, spheroids
- point and volume sources
- collects data by running a "scanning head" through predetermined scanning grid
- throws data to a CSV file

- geometry properties:
    - sources: activity, energy, later type?
    - blockers: blocking ratio per unit length (later also energy dependent)

- scanning head:
    - a scanning volume
    - interaction with scanning head dependent on path length thorugh the volume
    - for now basically a scintillator

- only works based on basic raytracing projection

- input data defined in JSON
- tooling for visualization of input geometry

- calculates all the rays first, then scans
    - speeds up calculation, no need to redo the whole simulation for each point sampled
