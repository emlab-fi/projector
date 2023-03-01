#pragma once
#include <optional>
#include <string>
#include <nlohmann/json.hpp>
#include "simulator.hpp"


std::optional<nlohmann::json> load_file(std::string filename);

std::optional<simulation_context> parse_config(const nlohmann::json& config);

bool save_data(std::unique_ptr<double[]> data, const simulation_context& sim_env, const std::string filename);