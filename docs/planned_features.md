- purely photon transport simulator (does not calculate secondary photons for now)
- geometry based on computational solid geometry

- input data defined in JSON
- tooling for visualization of input geometry

- calculates all the rays first, then tallies
    - speeds up calculation, no need to redo the whole simulation for each point sampled


# Cross section and form factor data
- uses standard ACE as input
- more specifically, the eprdata14 - https://nucleardata.lanl.gov/ace/eprdata14/
- used data:
    - coherent, incoherent, photoelectric absorption and pair production cross sections
    - coherent and incoherent form factors/functions
