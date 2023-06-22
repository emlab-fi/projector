#pragma once
#include <nlohmann/json.hpp>

namespace projector {

    void to_json(json& json, const json& json2);

} //namespace projector