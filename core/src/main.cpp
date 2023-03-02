#include <iostream>
#include <string>
#include <omp.h>
#include "CLI/CLI.hpp"
#include "geom.hpp"
#include "loader.hpp"
#include "simulator.hpp"

int main(int argc, char* argv[]) {

    CLI::App app{"Simple particle simulator"};

    std::string config_path;
    int thread_count = omp_get_max_threads();

    app.add_option("--input_file, -i, input_file", config_path, "Input json file")->required()->check(CLI::ExistingFile);
    app.add_option("--thread_count, -t", thread_count, "Max threads to use");

    CLI11_PARSE(app, argc, argv);


    std::cout << "<<-- Projector Core -->>" << std::endl;
    std::cout << "Loading config:" << config_path << std::endl;
    auto config_json = load_file(config_path);
    if (!config_json) {
        std::cerr << "Couldn't load config" << std::endl;
        return EXIT_FAILURE;
    }

    auto sim_env = parse_config(*config_json);
    if(!sim_env) {
        std::cerr << "Couldn't parse config" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Loaded experiment: " << config_json->at("experiment_name").get<std::string>() << std::endl;

    std::cout << "Starting simulation with " << thread_count << " threads" << std::endl;
    auto data = run_simulation(*sim_env, thread_count);

    bool saved = save_data(std::move(data), *sim_env, config_json->at("output_file").get<std::string>());
    if (!saved) {
        std::cerr << "Couldn't save data" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}