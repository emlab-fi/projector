#pragma once
#include <vector>
#include <string_view>
#include <filesystem>
#include "geometry.hpp"
#include "particle.hpp"
#include "material.hpp"

namespace projector {

struct object {
    std::string_view id;
    std::string_view material_string;
    std::size_t photons_activity;
    double  photons_energy;

    material_data material;
    geometry geom;
};


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
};


struct environment {
    std::string_view name;
    std::string_view description;
    std::filesystem::path output_path;
    uint64_t seed;
    bool save_particle_paths;

    double energy_cutoff;
    std::size_t stack_size;

    std::vector<object> objects;

    std::vector<tally> tallies;

    data_library cross_section_data;

    std::vector<particle> particles;
};

} //namespace projector