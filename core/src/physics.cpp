#include "physics.hpp"
#include "random_numbers.hpp"
#include "constants.hpp"

namespace {

double calculate_total_xs(const std::array<double, 5>& xs) {
    double total = 0.0;
    for (std::size_t i = 1; i < 5; ++i) {
        total += xs[i];
    }
    return total;
}

double get_xs(const std::array<double, 5>& data, projector::cross_section xs) {
    return data[static_cast<std::size_t>(xs)];
}

} // annonymous namespace


namespace projector {

void photon_interaction(particle& p, const element& element) {

    double current_energy = p.history.energies.back();

    std::array<double, 5> xs_data = element.get_all_cross_sections(current_energy);

    double total_xs = calculate_total_xs(xs_data);

    double prob = 0.0;
    double sample =  prng_double(p.prng_state) * total_xs;

    prob += get_xs(xs_data, cross_section::coherent);
    if (sample < prob) {

        double mu = element.rayleigh(p.history.energies.back(), p.prng_state);

        //sample phi for direction rotation
        double phi = 2.0 * constants::pi * prng_double(p.prng_state);

        p.current_direction = rotate_direction(p.current_direction, mu, phi);

        p.history.interactions.back() = cross_section::coherent;
        return;
    }

    prob += get_xs(xs_data, cross_section::incoherent);
    if (sample < prob) {

        auto [new_energy, mu] = element.compton(p.history.energies.back(), p.prng_state);

        p.history.energies.back() = new_energy;

        double phi = 2.0 * constants::pi * prng_double(p.prng_state);

        p.current_direction = rotate_direction(p.current_direction, mu, phi);

        p.history.interactions.back() = cross_section::incoherent;
        return;
    }

    prob += get_xs(xs_data, cross_section::photoelectric);
    if (sample < prob) {
        p.history.energies.back() = 0.0;
        p.history.interactions.back() = cross_section::photoelectric;
        return;
    }

    prob += get_xs(xs_data, cross_section::pair_production);
    if (sample < prob) {
        p.history.energies.back() = 0.0;
        p.history.interactions.back() = cross_section::pair_production;
        return;
    }
}

} // namespace projector