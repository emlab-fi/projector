## Geometric objects

### blocker/generic interaction volume
- contructed from primitive volumes
- interact by attenuating rays
- parameters
    - volume type/definition
    - attenuation (0.0 - 1.0)
    - (material/energy absorbtion)

### ParticlePath
- describes the simulated path of particle through environment
- consists of segments
- each segment:
    - start point
    - direction
    - length(t param)
    - start and end energy

## volume types

### axis aligned box
- defined by min and max point

### axis aligned cylinder
- centers of faces and radius

### axis aligned ellipsoid
- center + xyz radius
