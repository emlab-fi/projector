#pragma once
#include <vector>
#include "geometry.hpp"
#include "material.hpp"

namespace projector {

struct particle_history {
    std::vector<vec3> points;
    std::vector<double> energies;
    std::vector<cross_section> interactions;
};


struct particle {
    enum class type {
        photon,
        neutron,
        electron
    };

    type particle_type;

    vec3 current_direction;

    uint64_t prng_state;

    particle_history history;

    double& current_energy();

    vec3& current_position();

    void save_particle(const std::filesystem::path path) const;

    void photon_interaction(const element& elem);

    void advance(double macro_xs);

};



} // namespace projector