# Tallies
Tallies are the main output of the simulation.
They represent estimates of physical quantities in the simulation.

If the simulation is ran with multiple batches, the tallies represent the mean value of the individual batches.
The variance of the batches is also calculated during the simulation.

## Filters
Filters limit the input particles/interactions into the tally.
They are used to create tallies of specific variables/quantities in the simulation.
Currently supported filters are:

- particle source ID
- geometry ID
- interaction type
- interacted element
- particle energy range

Each tally type differs in how they work with the filters. Refer to the description of each type for details.

## Types

### Volume tally
Most basic tally.
It counts the score in the whole simulation volume, similar to creating uniform mesh tally with single cell spanning the whole volume.
It is designed to be used with filters to count specified physical quantities in the simulation.

Filter behaviour table (filters are evaluated top to bottom):
|Filter|Behaviour|
|:-----|:--------|
| particle source ID | Particle is ignored if it does not originate from specified source |
| geometry ID | Particle is ignored if it does not pass through specified geometry |
| interaction type | Particle is ignored if it does not have any interaction of this type in the specified geometry |
| interacted element | Particle is ignored if it does not interact with specified element inside the specified geometry |
| particle energy range | Particle is ignored if it does not fall into specified range when entering the specified geometry |

### Uniform mesh tally
Counts the score in a uniform, axis aligned grid of cells.
Each cell tracks the score separately.
The filter is recalculated for each cell.

Filter behaviour table (filters are evaluated top to bottom):
|Filter|Behaviour|
|:-----|:--------|
| particle source ID | Particle is ignored if it does not originate from specified source |
| geometry ID | Particle is ignored if the cell is not in specified geometry |
| interaction type | Particle is ignored if the track segment through the cell did not go through this interaction |
| interacted element | Particle is ignored if track segment through the cell did interact with this element |
| particle energy range | Particle is ignored if track segment through the cell is outside the energy range |

### Scintillator tally
Creates an energy histogram of total deposited energy by a particle.
By default tallies all particles, should be used with filters.
Can be used to create estimates of pulse height histogram.


Filter behaviour table (filters are evaluated top to bottom):
|Filter|Behaviour|
|:-----|:--------|
| particle source ID | Particle is ignored if it does not originate from specified source |
| geometry ID | Particle is ignored if it does not pass through specified geometry |
| interaction type | Particle is ignored if it does not have any interaction of this type in the specified geometry |
| interacted element | Particle is ignored if it does not interact with specified element inside the specified geometry |
| particle energy range | Particle is ignored if it does not fall into specified range when entering the specified geometry |


## Scores
Scores are the estimated physical quantities.
Projector currently supports following scores:

- Photon flux
- Interaction count
- Average energy
- Deposited energy

## Output
The tallies are saved into a `.csv` file at location specified in the configuration files.
The formatting of the file is given in the header of the file.
