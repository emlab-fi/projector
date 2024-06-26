# Input format

[TOC]

Projector uses [JSON](https://www.json.org/json-en.html) files for defining the simulation input.
The input is split into 4 JSON files.
This way, it's easier to reuse parts of the input data between multiple simulations and the files are also shorter and easier to read.
The files are split into a main file, material definitions, object definitions and tallies.
Various definitions in separate files are linked together using user-defined string IDs.

## Main file

The main file contains configuration of the simulator itself and the paths to other required files.

### Fields

|field|type|description|
|:----|:--:|:----------|
|`name`| `string` | name of the simulation |
|`description`| `string` | user description of the simulation |
|`save_particle_paths`| `bool` | Whether to save particle paths |
|`seed`| `uint` | PRNG seed |
|`energy_cutoff`| `float` | Energy cutoff value in kEv |
|`stack_size`| `uint` | The maximum history length for single particle |
|`bounding_box`| `[[float]]` | Array of min and max coordinates of the simulation (example bellow) |
|`material_file`| `string` | Path to the material JSON file |
|`object_file`| `string` | Path to the objects JSON file |
|`tallies_file`| `string` | Path to the tallies JSON file |
|`output_path` | `string` | Output path (where to save output data) |

Example of bounding box field:
```json
{
    "bounding_box": [[0.0, 0.0, 0.0], [2.5, 3.0, 1.0]]
}
```

## Material file

This file provides an array of material definitions like this:
```json
{
    "materials": [
        {/* material 1 */},
        {/* material 2 */},
        //......
    ]
}
```

### Fields

|field|type|description|
|:----|:--:|:----------|
|`id`|`string`| id of the material |
|`density` | `float` | Density of the material in \f$ g/cm^2 \f$ |
|`elements` | `[string]` | Array of elements in the material |
|`atomic_percentage` | `[float]` | Array of atomic percentages of the elements |
|`weight_percentage` | `[float]` | Array of weight percentages of the elements |

Elements are defined as their periodic table symbols.
Currently elements of atomic number up to 100 are supported (fermium).
There is a special element `void`, which means no element.
This can be used to simulate vacuum voids in materials or a vacuum in general.

When both atomic percentages and weight percentages are present, the atomic percentages are taken as input.
The percentages do not have to add up to 1, the values are normalized when loaded.
This means you can for example use `[1, 3]` as input and it will get normalized to `[0.25, 0.75]`. Care must be taken so that the percentage array is the same length as the elements array.

## Object file

This file defines the geometry of the simulation and links the geometry to used materials from the materials JSON file.
It's structure is bit more involved, as the geometry definitions can be recursive and are in a separate JSON object, compared to the object definition.

```json
{
    "geometries": {
        "geom1": { /* geom definition */ },
        "geom2": { /* geom definition */ },
        //......
    },
    "objects": [
        { /* object definition */ },
        { /* object definition */ },
        //......
    ]
}
```

### Geometry fields

|field|type|description|
|:----|:--:|:----------|
|`operators`|`[string]`| What operators to apply to the surfaces/geometries. Valid operations are `join`, `intersect`, `substract` |
|`surfaces`|`[string / object]`| The surfaces defining the geometry. It can also refer to another existing geometry. Surfaces are defined by two arguments, more on that below. |

When defining a surface, there are two arguments: `type` and `parameters`.
Below is table of possible surfaces and their parameters.
The parameters should be identical to the ones in [geometry docs](02_geometry.md) and are written as  they are accepted by a parser.

|surface|parameters|
|-------|----------|
|`plane`|`[x0, y0, z0], [a, b, c]`|
|`ellipsoid`|`[x0, y0, z0], a, b, c`|
|`x_cylinder`|`[x0, y0, z0], a, b`|
|`y_cylinder`|`[x0, y0, z0], a, b`|
|`z_cylinder`|`[x0, y0, z0], a, b`|
|`x_cone`|`[x0, y0, z0], a, b, c`|
|`y_cone`|`[x0, y0, z0], a, b, c`|
|`z_cone`|`[x0, y0, z0], a, b, c`|
|`box`|`[x0, y0, z0], [x1, y1, z1]`|
|`capped_x_cylinder`|`[x0, y0, z0], a, b, h`|
|`capped_y_cylinder`|`[x0, y0, z0], a, b, h`|
|`capped_z_cylinder`|`[x0, y0, z0], a, b, h`|
|`capped_x_cone`|`[x0, y0, z0], a, b, c, d1, d2`|
|`capped_y_cone`|`[x0, y0, z0], a, b, c, d1, d2`|
|`capped_z_cone`|`[x0, y0, z0], a, b, c, d1, d2`|


There should be no cyclic or self references when using geometries in place of surfaces, as this will not parse.

The operations array should be the same length as the surfaces array.
The surfaces and their operators are applied sequentially in the same order as they are defined.
So if we have surfaces `A, B, C` with operators `intersect, substract, join`, the end geometry will be `((_ intersect A) substract B) join C`.
The first surface should always have `intersect` operator.

It is best to see an example at the end of this page.

### Object fields

|field|type|description|
|:----|:--:|:----------|
|`id`|`string`| user defined object ID, mostly for easier identification in tallies |
|`geometry`|`string`| the name of the geometry for the object, must be a key in `geometries` |
|`material`|`string`| the object material ID, must be a valid material in the materials JSON file |
|`bounding_box`|`[[float]]`| optional bounding box of the object, if not present it's approximated automatically from the geometry (for an example see main file docs) |
|`source`|`json object`| optional object defining the particle source properties of the object |

When the `source` field is present, the object is considered as a source of particles.
The fields for the `source` are the following:

|field|type|description|
|:----|:--:|:----------|
|`photon_energy`|`float`| photon energy in MEv |
|`photon_count`|`uint`| how many photons originate from this source |
|`direction` |`[x, y, z]`| direction of the photons |
|`spread`|`float`|cosine of angle of maximum spread away from the direction |

## Tally file

This file describes the tallies to calculate.
The tallies are defined in similar manner to the material file, an array `tallies` containing the tally objects with the fields.

More details on tallies are here: [tallies](03_tallies.md).

### Fields

|field|type|description|
|:----|:--:|:----------|
|`id`|`string` |user supplied ID for easy identification |
|`type`| `string` | type of the tally, currently only `uniform_mesh` is supported |
|`score`| `string` | the physical quantity to evaluate |
|`parameters`| `object` | parameters for the tally, depend on the type |

The `parameters` entry is an object of more key-value pairs.
The keys depend on the type of the tally, short overview of required keys for tallies is bellow.

|tally type|required parameters|supported scores|
|:---------|:------------------|:---------------|
|`uniform_mesh`|`start`, `end`, `resolution`|`flux`, `average_energy`, `interaction_counts`, `deposited_energy`|
|`volume`|`object_id`|`flux`, `average_energy`, `interaction_counts`, `deposited_energy`|
|`scintillator`|`object_id`, `energy_range_bins`, `energy_max`||
