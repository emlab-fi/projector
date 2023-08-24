#pragma once
#include "geometry.hpp"
#include "material.hpp"
#include "particle.hpp"
#include "tally.hpp"

#include <string_view>
#include <string>
#include <vector>

namespace projector {

struct object {
    std::string id;
    std::size_t photons_activity;
    double photons_energy;

    std::size_t material_id;
    geometry geom;
};

struct environment {
    std::string name;
    std::string description;
    uint64_t seed;
    bool save_particle_paths;

    double energy_cutoff;
    std::size_t stack_size;
    bounding_box bounds;


    std::filesystem::path material_path;
    std::filesystem::path objects_path;
    std::filesystem::path tally_path;
    std::filesystem::path output_path;

    data_library cross_section_data;

    std::vector<std::string> material_ids;

    std::vector<material_data> materials;

    std::vector<object> objects;

    std::vector<std::unique_ptr<tally>> tallies;

    std::vector<particle> particles;
};

} // namespace projector