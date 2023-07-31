# Project input file formats

- json format - human readable and editable
- split into 3 files:
    - overview file - links to other files, generic sim settings
    - materials
    - objects
    - tallies
- links materials to objects by user string IDs

## Overview file

- name
- description
- save_particle_paths
- seed
- energy cutoff
- stack_size
- bounding_box (and it's material ID)
- material_file
- object_file
- tallies_file

## Material file
- list of materials
- each material:
    - ID
    - density
    - list of elements
    - list of atomic percentages OR weight percentages

## Object file
- list of geometries
    - geom ID
    - type - primitive or complex
    - if operation:
        - op
        - list of sub-geometries (can be IDs here?)
    - if primitive:
        - primitive type
        - list of coefficients

- list of objects
    - geometry ID
    - geometry shift/rotation?
    - material ID
    - optional source object
        - number of particles
        - energy of particles
        - direction - random or directed
        - if directed, then the direction vector and max deviation
    - optional helper bounding box, otherwise constructed automatically


## Tally file
- list of tallies
- each tally
    - filter - currently empty object
    - type - currently only uniform grid
    - score - the value to calculate
    - parameters - list of parameters (doubles)


## Example of the json
```json
{
    "name": "Experiment 1",
    "description": "Description of the experiment",
    "output": "./output/dir/path",
    "save_particle_paths": true,
    "seed": 1,
    "energy_cutoff": 100,
    "stack_size": 200,
    "objects": [
        {
            "id": "object id",
            "material": {
                "density": 1.23,
                "elements": ["He", "O", "U"],
                "atomic_percentage": [1, 2, 3],
                "weight_percentage": [80, 23.3, 11.2]
            },
            "photons_activity": 100,
            "photons_energy": 500000,
            "geometry": {
                "type" : "primitive|operation",
                "properties": {
                    "type": "aa_box|union|intersect|cut",
                    "parameters": [[100, 100], [20, 30]],
                    "left": { ... another geom},
                    "right": { ... another geom}
                }
            }
        },
        ...
    ],
    "tallies": {
        "type": "..."
        "cell_size": [10.2, 10.3, 10.4],
        "start": [0.0, 0.0, 1.0],
        "cell_count": [1, 2, 3]
    },
    ...
}
```
