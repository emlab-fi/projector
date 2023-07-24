#include "particle.hpp"
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


double& particle::current_energy() {
    return history.energies.back();
}


vec3& particle::current_position() {
    return history.points.back();
}


void particle::photon_interaction(const element& element) {

    std::array<double, 5> xs_data = element.get_all_cross_sections(current_energy());

    double total_xs = calculate_total_xs(xs_data);

    double prob = 0.0;
    double sample =  prng_double(prng_state) * total_xs;

    prob += get_xs(xs_data, cross_section::coherent);
    if (sample < prob) {

        double mu = element.rayleigh(current_energy(), prng_state);

        //sample phi for direction rotation
        double phi = 2.0 * constants::pi * prng_double(prng_state);

        current_direction = rotate_direction(current_direction, mu, phi);

        history.interactions.back() = cross_section::coherent;
        return;
    }

    prob += get_xs(xs_data, cross_section::incoherent);
    if (sample < prob) {

        auto [new_energy, mu] = element.compton(current_energy(), prng_state);

        current_energy() = new_energy;

        double phi = 2.0 * constants::pi * prng_double(prng_state);

        current_direction = rotate_direction(current_direction, mu, phi);

        history.interactions.back() = cross_section::incoherent;
        return;
    }

    prob += get_xs(xs_data, cross_section::photoelectric);
    if (sample < prob) {
        current_energy() = 0.0;
        history.interactions.back() = cross_section::photoelectric;
        return;
    }

    prob += get_xs(xs_data, cross_section::pair_production);
    if (sample < prob) {
        current_energy() = 0.0;
        history.interactions.back() = cross_section::pair_production;
        return;
    }
}

void particle::advance(double macro_xs) {

    double distance = -std::log(prng_double(prng_state)) / macro_xs;

    history.energies.push_back(current_energy());
    history.interactions.push_back(cross_section::no_interaction);
    history.points.push_back(current_position());

    current_position() += distance * current_direction;

}

} // namespace projector