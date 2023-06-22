#pragma once
#include <nlohmann/json.hpp>
#include "environment.hpp"

namespace projector {

NLOHMANN_JSON_SERIALIZE_ENUM(geom_primitive::shape, {
    {geom_primitive::shape::plane, "plane"},
    {geom_primitive::shape::aa_box, "aa_box"},
    {geom_primitive::shape::aa_ellipsoid, "aa_ellipsoid"},
})

NLOHMANN_JSON_SERIALIZE_ENUM(operation::type, {
    {operation::type::nop, "nop"},
    {operation::type::join, "join"},
    {operation::type::intersect, "intersect"},
    {operation::type::cut, "cut"},
})

NLOHMANN_JSON_SERIALIZE_ENUM(tally::type, {
    {tally::type::photon_energy_average, "photon_energy_average"},
    {tally::type::photon_flux, "photon_flux"},
    {tally::type::photon_interaction_counts, "photon_interaction_counts"}
})

void from_json(nlohmann::json& j, environment& env);

void from_json(nlohmann::json& j, tally& ta);

void from_json(nlohmann::json& j, object& obj);

void from_json(nlohmann::json& j, geometry& geom);

void from_json(nlohmann::json& j, geom_primitive& shape);

void from_json(nlohmann::json& j, operation& op);

void from_json(nlohmann::json& j, vec3& vec);

} //namespace projector