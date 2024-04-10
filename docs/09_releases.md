# Releases

Projector is currently in a beggining stages of development and thus has no stable release versions.
The nearest version is 0.1, and the preliminary roadmap of the versions and their features is below (checkmark signifies whether it is implemented in the main development branch).

## v0.1

- [x] polymorphic BREP geometry and CSG support
- [ ] proper unit testing of BREP, CSG
- [x] separate input to multiple files linked by one main file
- [ ] helper geometries
- [ ] implement support for void material/element
- [x] basic uniform mesh tally
- [x] add bounding box of environment
- [x] take bounding box into consideration in compute
- [ ] switch to eV from MeV units
- [ ] improve nearest surface calculations in x86 runtime
- [ ] parallelization in OpenMP
- [ ] basic documentation
- [x] clang format file

## v0.2

- [ ] rewrite talies
    - [ ] filters
    - [ ] various quantities
    - [ ] approximators?
    - [ ] statistical calculations
- [ ] separate sources from geometry objects (still can use objects as volume)
- [ ] add timestamp for events
- [ ] simulate more interactions (bremstrahhlung, proper compton?)
- [ ] batches, confidence/statistics calculation
- [ ] better python tooling
- [ ] better interface for different runtimes (CUDA etc.)

## v0.3

- [ ] look into mesh support (STL? OBJ?)
- [ ] non-x86 runtimes
- [ ] benchmarks
- [ ] verification, validation
- [ ] more generic ACE data parsing
- [ ] better output format
