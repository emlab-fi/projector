#include "particle.hpp"

#include "constants.hpp"
#include "random_numbers.hpp"

namespace {} // namespace

namespace projector {


double &particle::current_energy() { return history.energies.back(); }

vec3 &particle::current_position() { return history.points.back(); }

void particle::photon_interaction(const element &element) {

    sampled_xs xs_data = element.get_all_cross_sections(current_energy());

    double prob = 0.0;
    double sample = prng_double(prng_state) * xs_data.total;

    history.elements.back() = element.atomic_number;

    prob += xs_data.coherent;
    if (sample < prob) {

        double mu = element.rayleigh(current_energy(), prng_state);

        // sample phi for direction rotation
        double phi = 2.0 * constants::pi * prng_double(prng_state);

        current_direction = rotate_direction(current_direction, mu, phi);

        history.interactions.back() = cross_section::coherent;
        return;
    }

    prob += xs_data.incoherent;
    if (sample < prob) {

        auto [new_energy, mu] = element.compton(current_energy(), prng_state);

        current_energy() = new_energy;

        double phi = 2.0 * constants::pi * prng_double(prng_state);

        current_direction = rotate_direction(current_direction, mu, phi);

        history.interactions.back() = cross_section::incoherent;
        return;
    }

    prob += xs_data.photoelectric;
    if (sample < prob) {
        current_energy() = 0.0;
        history.interactions.back() = cross_section::photoelectric;
        return;
    }

    prob += xs_data.pair_production;
    if (sample < prob) {
        current_energy() = 0.0;
        history.interactions.back() = cross_section::pair_production;
        return;
    }
}

void particle::advance(double distance) {

    history.energies.push_back(current_energy());
    history.interactions.push_back(cross_section::no_interaction);
    history.elements.push_back(history.elements.back());

    vec3 new_position = current_position() + distance * current_direction;

    history.points.push_back(new_position);
}

} // namespace projector