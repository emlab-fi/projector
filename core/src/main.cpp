#include "CLI/CLI.hpp"
#include "environment.hpp"
#include "json_loader.hpp"
#include "runtime.hpp"
#include "utils.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <string_view>
#include <termcolor/termcolor.hpp>

int main(int argc, char *argv[]) {

    CLI::App app{"Projector - photon transport simulator"};

    std::string_view config_path_str;
    std::string_view xcom_path;
    std::string_view xsdir_path;
    int thread_count = 1;

    app.add_option("--input_file, -i", config_path_str, "Input JSON file")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("--ace_data, -a", xsdir_path, "Path to eprdata14 xsdir")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("--thread_count, -t", thread_count, "Max threads to use");

    CLI11_PARSE(app, argc, argv);


    std::cout << termcolor::blue << termcolor::bold << "<<-- Projector -->>"
              << termcolor::reset << std::endl;

    projector::environment sim_env;

    std::cout << "Loading cross section data from: " << xsdir_path << std::endl;
    try {
        sim_env.cross_section_data =
            projector::data_library::load_ace_data(xsdir_path);
    } catch (const std::exception &e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }

    std::filesystem::path config_path(config_path_str);

    try {
        std::cout << "Loading input file: " << config_path << std::endl;
        projector::load_simulation_data(config_path, sim_env);

        std::filesystem::path material_path = config_path;
        material_path.replace_filename(sim_env.material_path);

        std::cout << "Loading material file: " << material_path << std::endl;
        projector::load_material_data(material_path, sim_env);

        std::filesystem::path object_path = config_path;
        object_path.replace_filename(sim_env.objects_path);

        std::cout << "Loading objects file: " << object_path << std::endl;
        projector::load_object_data(object_path, sim_env);

        std::filesystem::path tally_path = config_path;
        tally_path.replace_filename(sim_env.tally_path);

        std::cout << "Loading tally file: " << tally_path << std::endl;
        projector::load_tally_data(tally_path, sim_env);

    } catch (const std::exception &e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }

    std::cout << "Loaded all files successfully" << std::endl;
    std::cout << "Experiment: " << sim_env.name << std::endl;
    std::cout << "Description: " << sim_env.description << std::endl;

    std::cout << "Initializing runtime" << std::endl;
    projector::initialize_runtime(sim_env, thread_count);

    std::cout << "Running particle simulation" << std::endl;
    projector::calculate_particle_histories(sim_env);

    std::cout << "Processing tallies" << std::endl;
    // projector::process_tallies(sim_env);

    std::cout << "Saving data to: " << sim_env.output_path << std::endl;
    projector::save_data(sim_env);

    return EXIT_SUCCESS;
}