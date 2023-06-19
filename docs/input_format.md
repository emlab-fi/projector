# projector input format

- json format - somewhat human readable

## Example of the json
```json
{
    "name": "Experiment 1",
    "description": "Description of the experiment",
    "output": "./output/dir/path",
    "save_particle_paths": true,
    "seed": -1,
    "objects": [
        {
            "id": "object id",
            "properties": {
                "material": "H20" | "CO2" | [["H", 2.11], ["He", 3.28]],
                "activity_photons": 100,
                "energy_photons": 500000
            },
            "geometry": {
                "type" : "primitive|union|intersect|cut",
                "properties": {
                    "shape": "aa_box",
                    "parameters": [[100, 100], [20, 30]],
                    "left": { ... another geom},
                    "right": { ... another geom}
                }
            }
        },
        ...
    ]
}
```
