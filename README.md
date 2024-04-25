# Projector

Simple monte carlo based high energy (>1 kEv) photon transport simulator.
Developed as by Oldřich Pecák in EmLab during his PhD.
Aim of the simulator is to be a simplified simulator which can be used as a performance proxy application.

Algorithmically similar to common standard simulators (MCNP, OpenMC, PENELOPE, Serpent).

Currently uses [eprdata14](https://nucleardata.lanl.gov/ace/eprdata14/) ACE as cross section source.

### Structure
- `core` - the main compute core of the simulator
- `docs` - documentation
- `tooling` - python tooling for running the simulator and data visualization

### License

MIT license, see LICENSE file.
