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
    "geometry": [
        {
            "id": "object id",
            "properties": {
                "material": [["H", 2], ["O", 1]],
                "activity_photons": 100,
                "energy_photons": 500000
            },
            "shape": "aa-box",
            "shape_properties": {
                "coeffs": [ [3, 1, 2], [4, 2, 3] ],
                "union/intersect/cut": {
                    "shape": "z-cylinder",
                    "coeffs": [ [], [], [] ],
                    "cut": ...
                }
            }
        },
        ...
    ]
}
```
