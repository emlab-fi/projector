# Projector user guide

[TOC]

## Installation

Currently we do not provide pre-built executables for Projector.
The recommended way to install Projector is to build it from either latest development branch or from the main release branch.

1. Download source of desired version
2. Compile the core component
3. Either move the built executable to a directory in `PATH` so that it is globally accessible or use it directly from the build folder.
4. Download [eprdata14](https://nucleardata.lanl.gov/ace/eprdata14/) ACE files
5. Create an environment variable called `PROJECTOR_ACE_XSDIR` which contains path to the `xsdir` file of the `eprdata14` library.
    - If you skip this step, you need to provide the path to the library every time you launch Projector executable.

## Building from source

The build for Projector is quite selfcontained. The only dependencies are `CMake 3.12` or newer and C++ compiler supporting C++20.
The build is currently only tested on `GCC 13.2` and `CMake 3.27`, on `Ubuntu 23.10`

### Building on Linux

```console
$ cd core
$ mkdir build && mv build
$ cmake ..
$ make projector_core
```

### Building on Windows

Not yet tested, but should work.

## General workflow

The general workflow for simulating a problem in Projector is as follows.
Some steps are not yet fully implemented and can change between releases.

1. Define problem in input json files - see [input format](06_input_format.md) - recommended order is:
    1. Main configuration file
    2. Materials
    3. Geometry
    4. Tallies
2. Launch projector_core executable - this step can take a long time!
3. Examine results using tooling - most tooling is currently missing and is not implemented well.

## Launching projector

The main Projector executable is called `projector_core`.
It can run the simulation, validate input files and create data for plots of the geometry.
It takes several arguments and options, described below.

```
Usage: .projector_core [OPTIONS] SUBCOMMAND INPUT

Positionals:
  INPUT    Input JSON file [REQUIRED]- the main configuration JSON file

Options:
  -h,--help                   print help message and exit
  -a,--ace_data               path to eprdata14 xsdir - can ommit if provided in PROJECTOR_ACE_XSDIR env variable
  -t,--thread_count INT       max threads to use

Subcommands:
  run                         run simulation from input files
  validate                    validates input files
  visualize                   plot input geometry
```

The visualization subcommand has several more options:
```
  -s,--slice      [REQUIRED]  slice plane - must be one of x,y,z
  -c,--center                 center point of the slice, default value is 0.0
  -r,--resolution [REQUIRED]  output resolution, must be two positive integer numbers
  -o,--output                 output .csv file, default value is 'slice_out.csv'
```

## Python tooling

Future goal of Projector is to provide a set of scripts in Python for easier work with the main executable and for post-processing/visualization of tally results.
The beggings of the tooling implementation are currently in the `tooling` folder of the repository.