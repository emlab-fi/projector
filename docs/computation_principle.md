# Projector computation principle

## Sample the volume to create particle origin
- sample PRNG for position and energy distribution
- only done once for each particle

## Calculate closest object
- can be inside of an object
- accelerated using axis aligned bounding boxes
- only unidirectional - particle only goes forward

## Calculate resulting interaction segment with object
- calculate intersection segment (whole, start to finish)
- extend current segment to start of interaction segment
- calculate the interaction probability (per unit length) (dependent on energy and object material)
- calculate length of intersection
- sample PRNG and determine point of interaction (if any)
- sample PRNG and determine interaction type (dependent on material and energy)
- do the calculations for interaction, if it is scattering, modify direction
- if the resulting interaction is absorbent, terminate the computation of the particle

## Interactions calculation

## Photoeletric Absorption
- as we do not care about resulting particles, the photon just gets stopped

## Coherent Scattering (Rayleigh)
-

## Incoherent Scattering (Compton)
-

## Pair Production
- as we do not care about resulting particles, the photon just gets stopped
