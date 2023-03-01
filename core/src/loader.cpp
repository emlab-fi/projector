#include <fstream>
#include <iostream>
#include <stdexcept>
#include "loader.hpp"

using json = nlohmann::json;

std::optional<json> load_file(std::string filename) {
    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        std::cerr << "Failed opening file" << std::endl;
        return {};
    }

    json parsed = json::parse(input_file, nullptr, false, false);

    input_file.close();

    if (parsed.is_discarded()) {
        std::cerr << "Couldn't parse JSON" << std::endl;
        return {};
    }
    return parsed;
}

std::optional<simulation_context> parse_config(const json& config) {
    simulation_context env;
    try {
        config.get_to(env);
    } catch (std::exception const& ex) {
        std::cerr << ex.what() << std::endl;
        return {};
    }
    return env;
}

bool save_data(std::unique_ptr<double[]> data, const simulation_context& sim_env, const std::string filename) {
    std::fstream output(filename, output.out|output.trunc);

    if (!output.is_open()) {
        std::cerr << "failed opening output file" << std::endl;
        return false;
    }

    size_t x_steps = (sim_env.stop.x - sim_env.start.x) / sim_env.resolution;
    size_t y_steps = (sim_env.stop.y - sim_env.start.y) / sim_env.resolution;

    for (size_t y = 0; y < y_steps; ++y) {
        for (size_t x = 0; x < x_steps; ++x) {
            size_t index = y * x_steps + x;
            double x_pos = sim_env.start.x + x * sim_env.resolution;
            double y_pos = sim_env.start.y + y * sim_env.resolution;
            output << x_pos << "," << y_pos << "," << data[index] << std::endl;
        }
    }

    output.close();
    return true;
}