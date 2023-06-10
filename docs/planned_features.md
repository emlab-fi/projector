- purely photon transport simulator (does not calculate secondary photons for now)
- axis aligned boxes for geometry
- also axis aligned cylinders, spheroids
- point and volume sources
- collects data by running a "scanning head" through predetermined scanning grid
- throws data to a CSV file (or HDF5?)
- interaction data pulled from ACE data

- geometry - defined by material, combined with calculations from XCOM database

- scanning head:
    - a scanning volume
    - interaction with scanning head dependent on path length thorugh the volume
    - for now basically a scintillator

- only works based on basic raytracing projection

- input data defined in JSON
- tooling for visualization of input geometry

- calculates all the rays first, then scans
    - speeds up calculation, no need to redo the whole simulation for each point sampled
