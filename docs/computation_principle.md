# Projector computation principle

## Single particle simulation

0. Sample the volume to create particle origin
    - sample PRNG for position and energy distribution
    - only done once for each particle
1. Calculate distance to next collision
    - exponentional function
    - check if we crossed surface boundary and possibly reflect back
    - if we cross a boundary, calculate a new distance
2. Determine volume, sample crosssections
3. Calculate interactions
4. Modify particle values, save to particle history
5. Repeat from 1. unless we reach max particle stack size, energy cutoff threshold or total particle absorption


# Interactions calculation

- calculate total crossection value
- sample uniform random variable between 0 and total crosssection
- determine interaction

## Photoeletric Absorption
- as we do not care about resulting particles, the photon just gets stopped

## Coherent Scattering (Rayleigh)
-

## Incoherent Scattering (Compton)
-

## Pair Production
- as we do not care about resulting particles, the photon just gets stopped/absorbed
