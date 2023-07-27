# projector input format

- json format - somewhat human readable
- later split to multiple files?

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
