#pragma once
#include <filesystem>
#include "particle.hpp"

namespace projector {

struct tally {
    enum class type {
        photon_flux,
        photon_energy_average,
        photon_interaction_counts,
        interaction_distance_average,
    };

    vec3 cell_size;
    vec3 start;

    std::size_t x_count;
    std::size_t y_count;
    std::size_t z_count;

    type tally;

    std::vector<double> results;
    std::size_t stride;

    void save_tally(const std::filesystem::path path) const;

    void init_tally();

    void add_particle(const particle& p);

    void process_data();
};

} // namespace projector