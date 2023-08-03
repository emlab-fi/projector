#pragma once
#include "environment.hpp"

#include <nlohmann/json.hpp>

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

void from_json(nlohmann::json &j, environment &env);

void from_json(nlohmann::json &j, tally &ta);

void from_json(nlohmann::json &j, object &obj);

void from_json(nlohmann::json &j, material_data &mat);

void from_json(nlohmann::json &j, geometry &geom);

//void from_json(nlohmann::json &j, geom_primitive &shape);

//void from_json(nlohmann::json &j, operation &op);

void from_json(nlohmann::json &j, vec3 &vec);

} // namespace projector