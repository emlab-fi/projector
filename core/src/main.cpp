#include "CLI/CLI.hpp"
#include "environment.hpp"
#include "json_converters.hpp"
#include "runtime.hpp"
#include "utils.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <string_view>

int main(int argc, char *argv[]) {

    CLI::App app{"Projector - photon transport simulator"};

    std::string_view config_path;
    std::string_view xcom_path;
    std::string_view xsdir_path;
    int thread_count = 1;

    app.add_option("--xcom, -x", xcom_path, "XCOM database path (deprecated)")
        ->check(CLI::ExistingPath);
    app.add_option("--ace_xsdir, -a", xsdir_path,
                   "xsdir file path for ACE (eprdata14)")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("--input_file, -i, input_file", config_path,
                   "Input json file")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("--thread_count, -t", thread_count, "Max threads to use");

    CLI11_PARSE(app, argc, argv);


    std::cout << "<<-- Projector -->>" << std::endl;

    projector::environment sim_env;

    std::cout << "Loading input file: " << config_path << std::endl;

    nlohmann::json config_json;
    try {
        config_json = load_json_file(config_path);
    } catch (const std::exception &e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }

    try {
        //from_json(config_json, sim_env);
    } catch (const std::exception &e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }

    std::cout << "Loaded config file successfully" << std::endl;
    std::cout << "Experiment: " << sim_env.name << std::endl;


    std::cout << "Loading cross section data from: " << xsdir_path << std::endl;
    try {
        sim_env.cross_section_data =
            projector::data_library::load_ace_data(xsdir_path);
    } catch (const std::exception &e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }
    std::cout << "Loaded cross section data successfully" << std::endl;

    std::cout << "Processing material data" << std::endl;
    for (auto &obj : sim_env.objects) {
        sim_env.cross_section_data.material_calculate_missing_values(
            obj.material);
    }

    std::cout << "Initializing runtime" << std::endl;
    projector::initialize_runtime(sim_env, thread_count);

    std::cout << "Running particle simulation" << std::endl;
    projector::calculate_particle_histories(sim_env);

    std::cout << "Processing tallies" << std::endl;
    projector::process_tallies(sim_env);

    std::cout << "Saving data to: " << sim_env.output_path << std::endl;
    projector::save_data(sim_env);

    return EXIT_SUCCESS;
}