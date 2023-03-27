# reactor imput format

- json format - somewhat human readable

## Example of the json
```json
{
    "name": "Experiment 1",
    "description": "Description of the experiment",
    "output": "./output/dir/path",
    "save_particle_paths": true,
    "objects": [
        {
            "id": "object id",
            "properties": {
                "absorption": 0.123,
                "activity": 100,
                "energy": 500000
            },
            "shape": "aa_box",
            "shape_properties": {
                "min_point": [3, 1, 2],
                "max_point": [4, 2, 3]
            }
        }
    ],
    "fixture": {
        "type": "scintillator",
        "properties": {
            "absorption": 0.314,
        },
        "scan_grid": {
            "start": [0, 0, 0],
            "stop": [2, 2, 2],
            "steps": [10, 10, 1]
        },
        "shape": "aa_cyl",
        "shape_properties" {
            "start": [0, 0, 0],
            "axis": [1, 0, 0],
            "dimensions": [1, 2, 3]
        }
    }
}
```
