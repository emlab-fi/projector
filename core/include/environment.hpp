#pragma once
#include "geometry.hpp"
#include "material.hpp"
#include "particle.hpp"
#include "tally.hpp"

#include <string_view>
#include <vector>
#include <map>

namespace projector {

struct object {
    std::string_view id;
    std::size_t photons_activity;
    double photons_energy;

    std::size_t material_id;
    geometry geom;
};

struct environment {
    std::string_view name;
    std::string_view description;
    uint64_t seed;
    bool save_particle_paths;

    double energy_cutoff;
    std::size_t stack_size;
    std::pair<vec3, vec3> top_level_bounding_box;


    std::filesystem::path material_path;
    std::filesystem::path objects_path;
    std::filesystem::path tally_path;
    std::filesystem::path output_path;

    data_library cross_section_data;

    std::vector<std::string_view> material_ids;

    std::map<std::size_t, material_data> materials;

    std::vector<object> objects;

    std::vector<tally> tallies;

    std::vector<particle> particles;
};

} // namespace projector