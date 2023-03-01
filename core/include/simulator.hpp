#pragma once
#include <vector>
#include <utility>
#include <memory>
#include <nlohmann/json.hpp>
#include "geom.hpp"

enum class scan_type {
    sensor_source_shift,
    sensor_shift,
};

struct blocker {
    double attenuation;
    geom::z_area_2d shape;
    std::vector<geom::z_area_2d> holes;
};

struct simulation_context {
    scan_type type;
    geom::point start;
    geom::point stop;
    double resolution;
    std::pair<double, double> noise_coefficients;
    std::pair<double, double> scintillator_size;
    geom::point source;
    unsigned int source_activity;
    std::vector<blocker> blockers;
};

std::unique_ptr<double[]> run_simulation(simulation_context& env, int thread_count);

void from_json(const nlohmann::json& j, blocker& b);
void from_json(const nlohmann::json& j, simulation_context& env);