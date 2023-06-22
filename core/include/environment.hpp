#pragma once
#include <vector>
#include <string_view>
#include <filesystem>
#include "geometry.hpp"
#include "cross_sections.hpp"

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
    };

    vec3 cell_size;
    vec3 start;

    std::size_t x_count, y_count, z_count;

    type tally;
};

struct environment {
    std::string_view name;
    std::string_view description;
    std::filesystem::path output_path;
    uint64_t seed;
    bool save_particle_paths;

    std::vector<object> objects;

    std::vector<tally> tallies;
};

} //namespace projector