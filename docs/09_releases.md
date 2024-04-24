# Releases

Projector is currently in a beggining stages of development and thus has no stable release versions.
The nearest version is 0.2, and the preliminary roadmap of the versions and their features is below (checkmark signifies whether it is implemented in the main development branch).
They are a mix of actual features and development targets.

## v0.3 (possibly v1.0)

- [ ] rewrite cross section storage, implement union energy grid
    - [ ] do not load all elements?
- [ ] look into mesh support (STL? OBJ?)
- [ ] non-x86 runtimes
- [ ] benchmarks
- [ ] verification, validation
- [ ] more generic ACE data parsing
- [ ] better output format

## v0.2

- [ ] rewrite talies
    - [ ] filters
    - [ ] various quantities
    - [ ] approximators?
    - [ ] statistical calculations
- [ ] fix/improve unit testing
- [ ] separate sources from geometry objects (still can use objects as volume)
- [ ] object instancing/grids
- [ ] add timestamp for events
- [ ] switch to eV from MeV, cm2 from barns
    - [ ] investigate why it fails at some instances
- [ ] simulate more interactions (bremstrahhlung, proper compton?)
- [ ] batches, confidence/statistics calculation
- [ ] better python tooling
    - [ ] unified visualization tool
    - [ ] loading from input files rather than direct output
- [ ] better interface for different runtimes (CUDA etc.)

## v0.1 - Released 26/4/2024

- [x] polymorphic BREP geometry and CSG support
- [x] separate input to multiple files linked by one main file
- [x] helper geometries
- [x] basic uniform mesh tally
- [x] add bounding box of environment
- [x] take bounding box into consideration in compute
- [x] improve nearest surface calculations in x86 runtime
- [x] geometry visualization
- [x] parallelization in OpenMP
- [x] basic documentation
- [x] clang format file
