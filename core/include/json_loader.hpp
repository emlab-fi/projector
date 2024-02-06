#pragma once
#include "environment.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>

namespace projector {

NLOHMANN_JSON_SERIALIZE_ENUM(csg_operation, {
                                                {csg_operation::no_op, "no_op"},
                                                {csg_operation::join, "join"},
                                                {csg_operation::intersect, "intersect"},
                                                {csg_operation::substract, "substract"},
                                            })


NLOHMANN_JSON_SERIALIZE_ENUM(tally_score,
                             {
                                 {tally_score::flux, "flux"},
                                 {tally_score::average_energy, "average_energy"},
                                 {tally_score::interaction_counts, "interaction_counts"},
                                 {tally_score::deposited_energy, "deposited_energy"}
                             }

)

std::unique_ptr<surface> parse_surface(nlohmann::json &j);

void parse_geometry(geometry &geom, std::string_view key, nlohmann::json &j);

void load_simulation_data(std::filesystem::path path, environment &env);

void load_material_data(std::filesystem::path path, environment &env);

void load_object_data(std::filesystem::path path, environment &env);

void load_tally_data(std::filesystem::path path, environment &env);

} // namespace projector