# Tallies
Tallies are the main output of the simulation.
They represent estimates of physical quantities in the simulation.

If the simulation is ran with multiple batches, the tallies represent the mean value of the individual batches.
The variance of the batches is also calculated during the simulation.

## Types

### Volume tally
Most basic tally.
It counts the score in the whole simulation volume, similar to creating uniform mesh tally with single cell spanning the whole volume.
It is designed to be used with filters to count specified physical quantities in the simulation.

### Uniform mesh tally
Counts the score in a uniform, axis aligned grid of cells.
Each cell tracks the score separately.

### Scintillator tally
Creates an energy histogram of total deposited energy by a particle.
By default tallies all particles, should be used with filters.
Can be used to create estimates of pulse height histogram.

## Filters
Filters limit the input particles/interactions into the tally.
They are used to create tallies of specific variables/quantities in the simulation.
Currently supported filters are:

- particle energy range
- interacted element
- interaction type
- geometry ID
- material ID
- particle source ID

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
