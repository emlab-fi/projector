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


vec3 vec3_from_json(nlohmann::json &j);

std::unique_ptr<surface> parse_surface(nlohmann::json &j);

void parse_geometry(geometry &geom, std::string_view key, nlohmann::json &j);

void load_simulation_data(std::filesystem::path path, environment &env);

void load_material_data(std::filesystem::path path, environment &env);

void load_object_data(std::filesystem::path path, environment &env);

void load_tally_data(std::filesystem::path path, environment &env);

} // namespace projector