#include "CLI/CLI.hpp"
#include "constants.hpp"
#include "environment.hpp"
#include "json_loader.hpp"
#include "plot.hpp"
#include "runtime.hpp"
#include "utils.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <string_view>
#include <termcolor/termcolor.hpp>

int main(int argc, char *argv[]) {

    CLI::App app{"Projector - photon transport simulator"};
    CLI::App &run_subcommand = *app.add_subcommand("run", "run simulation from input files");
    CLI::App &parse_subcommand = *app.add_subcommand("validate", "validates input files");
    CLI::App &vis_subcommand = *app.add_subcommand("visualize", "plot input geometry");

    std::string config_path_str;
    std::string xcom_path;
    std::string xsdir_path;
    int thread_count = 1;

    double vis_center;
    std::vector<std::size_t> vis_resolution;
    char vis_plane;
    std::string vis_output_path;

    app.require_subcommand(1);
    app.add_option("--ace_data, -a", xsdir_path, "Path to eprdata14 xsdir")
        ->envname("PROJECTOR_ACE_XSDIR")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("--thread_count, -t", thread_count, "Max threads to use");
    app.set_help_all_flag("--help-all", "Show help for all subcommands");

    vis_subcommand.add_option("-s, --slice", vis_plane, "Slice plane")
        ->check(CLI::IsMember({'x', 'y', 'z'}))
        ->required();
    vis_subcommand.add_option("-c, --center", vis_center, "Center point of the slice")
        ->default_val(0.0);
    vis_subcommand.add_option("-r, --resolution", vis_resolution, "output resolution")
        ->expected(2)
        ->required();
    vis_subcommand.add_option("-o, --output", vis_output_path, "output_path")
        ->default_val("slice_out.csv");
    vis_subcommand.add_option("INPUT", config_path_str, "Input JSON file")
        ->required()
        ->check(CLI::ExistingFile);

    parse_subcommand.add_option("INPUT", config_path_str, "Input JSON file")
        ->required()
        ->check(CLI::ExistingFile);

    run_subcommand.add_option("INPUT", config_path_str, "Input JSON file")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    std::cout << termcolor::blue << termcolor::bold << "<<-- Projector -->>" << termcolor::reset
              << std::endl;

    projector::environment sim_env;

    std::cout << "Loading cross section data from: " << xsdir_path << std::endl;
    try {
        sim_env.cross_section_data = projector::data_library::load_ace_data(xsdir_path);
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
    std::cout << "Epsilon size: " << projector::constants::epsilon << std::endl;

    if (parse_subcommand) {
        std::cout << "Validate only run - exiting" << std::endl;
        return EXIT_SUCCESS;
    }


    if (vis_subcommand) {
        std::cout << "Plotting geometry" << std::endl;

        std::cout << "Slice plane: " << vis_plane << std::endl;
        std::cout << "Slice center: " << vis_center << std::endl;

        std::cout << std::endl;
        std::cout << "Resolution: " << vis_resolution[0] << " " << vis_resolution[1] << std::endl;

        // run visualization
        projector::env_slice_plot(sim_env, vis_plane, vis_center, vis_resolution[0],
                                  vis_resolution[1], vis_output_path);

        std::cout << "Saved slice to: " << vis_output_path << std::endl;
        return EXIT_SUCCESS;
    }


    if (run_subcommand) {
        std::cout << "Initializing runtime" << std::endl;
        projector::initialize_runtime(sim_env, thread_count);

        std::cout << "Running particle simulation" << std::endl;
        projector::calculate_particle_histories(sim_env);

        std::cout << "Processing tallies" << std::endl;
        projector::process_tallies(sim_env);

        std::cout << "Saving data to: " << sim_env.output_path << std::endl;
        projector::save_data(sim_env);
    }

    std::cout << "Invalid subcommand, exiting" << std::endl;
    return EXIT_FAILURE;
}