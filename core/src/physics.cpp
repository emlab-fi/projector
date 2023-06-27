#include "physics.hpp"
#include "random_numbers.hpp"

namespace {

};

namespace projector {

void photon_interaction(particle& p, const cross_section_sample& cross_sections) {

    double prob = 0.0;
    double sample =  prng_double(p.prng_state) * cross_sections.total;

    prob += cross_sections.coherent;
    if (sample < prob) {
        //TODO
    }

    prob += cross_sections.incoherent;
    if (sample < prob) {

    }

    prob += cross_sections.photoelectric;
    if (sample < prob) {

    }

    prob += cross_sections.pair_nuclear;
    if (sample < prob) {

    }

    prob +=cross_sections.pair_electron;
    if (sample < prob) {

    }
}

} // namespace projector