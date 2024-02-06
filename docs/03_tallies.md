# Tallies

## Output
The tallies are saved into a `.csv` file at location specified in the configuration files.

## Uniform mesh tallies

These tallies are counted in a uniform grid.
Each quantity is then tallied in each cell of the grid.

### Algorithm for tallying a single particle

For each segment of particle history, do the following:

1. determine whether the segment actually goes through the tally space
2. calculate all grid crossing points of the segment
3. for each grid cross point:
    1. determine corresponding tally cell coordinates
    2. tally the requested value

This should be linear per each segment.
Worst case the segment hits at most the number of cells on the longest grid dimension.

This should also be able to be done on all mesh tallies at once, no need to sequentially process each tally.

### Scores
The score is the physical quantity to evualate.
Currently supported scores are:

- Photon flux
- Number of reactions
- Average energy
- Deposited energy

## Visualization tally

This tally is a helper tally to visualize the geometry of the environment.
It divides the whole simulation environment bounding box into grid and looking up the material/object at center of each grid cell.
The input parameters are the X, Y and Z resolution (number of grid cells per axis).
