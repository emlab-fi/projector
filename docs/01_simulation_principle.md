# Projector simulation principle

- general flow:

1. load input data (cross sections, simulation config)
2. initialize simulation runtime
    - pre-allocate memory for particle histories
    - precompute needed stuff
3. run particle history simulation, save the history
4. calculate tallies over the particle histories
5. save data

## Single particle simulation

1. Sample the volume to create particle origin
    - sample PRNG for position
    - only done once for each particle
2. Calculate distance to next collision
    - exponentional function
    - check if we crossed surface boundary and possibly reflect back
    - if we cross a boundary, calculate a new distance
3. Determine volume, sample crosssections
4. Calculate interactions
5. Modify particle values, save to particle history
6. Repeat from 1. unless we reach max particle stack size, energy cutoff threshold or total particle absorption


## Interactions calculation

- calculate total crossection value
- sample uniform random variable between 0 and total crosssection
- determine interaction

### Photoeletric Absorption
- as we do not care about secondary particles, the photon just gets stopped

### Coherent Scattering (Rayleigh)
-

### Incoherent Scattering (Compton)
-

### Pair Production
- as we do not care about secondary particles, the photon just gets stopped/absorbed
