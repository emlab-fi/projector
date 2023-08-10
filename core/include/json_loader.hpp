#pragma once
#include "environment.hpp"

#include <nlohmann/json.hpp>
#include <filesystem>

namespace projector {

NLOHMANN_JSON_SERIALIZE_ENUM(csg_operation,
                             {
                                 {csg_operation::no_op, "no_op"},
                                 {csg_operation::join, "join"},
                                 {csg_operation::intersect, "intersect"},
                                 {csg_operation::substract, "substract"},
                             })

NLOHMANN_JSON_SERIALIZE_ENUM(
    tally::type,
    {
        {tally::type::photon_energy_average, "photon_energy_average"},
        {tally::type::photon_flux, "photon_flux"},
        {tally::type::photon_interaction_counts, "photon_interaction_counts"},
        {tally::type::interaction_distance_average,
         "interaction_distance_average"},
    })

vec3 vec3_from_json(nlohmann::json &j);

void parse_geometry(geometry &geom, std::string_view key, nlohmann::json &j);

void load_simulation_data(std::filesystem::path path, environment &env);

void load_material_data(std::filesystem::path path, environment &env);

void load_object_data(std::filesystem::path path, environment &env);

void load_tally_data(std::filesystem::path path, environment &env);

} // namespace projector