#pragma once
#include "geometry.hpp"
#include "material.hpp"

#include <vector>

namespace projector {

struct particle_history {
    std::vector<vec3> points;
    std::vector<double> energies;
    std::vector<cross_section> interactions;
    std::vector<std::size_t> elements;
};

struct particle {
    enum class type { photon, positron, electron };

    type particle_type;

    vec3 direction;

    uint64_t prng_state;

    particle_history history;

    double &energy();

    vec3 &position();

    void save_particle(const std::filesystem::path path) const;

    void photon_interaction(const element &elem);

    void advance(double distance);
};


} // namespace projector