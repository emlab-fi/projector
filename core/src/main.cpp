#include <iostream>
#include <string_view>
#include <nlohmann/json.hpp>
#include "CLI/CLI.hpp"
#include "utils.hpp"
#include "environment.hpp"
#include "json_converters.hpp"


int main(int argc, char* argv[]) {

    CLI::App app{"Projector - photon transport simulator"};

    std::string_view config_path;
    std::string_view xcom_path;
    std::string_view xsdir_path;
    int thread_count = 1;

    app.add_option("--xcom, -x", xcom_path, "XCOM database path (deprecated)")->check(CLI::ExistingPath);
    app.add_option("--ace_xsdir, -a", xsdir_path, "xsdir file path for ACE (eprdata14)")->required()->check(CLI::ExistingFile);
    app.add_option("--input_file, -i, input_file", config_path, "Input json file")->required()->check(CLI::ExistingFile);
    app.add_option("--thread_count, -t", thread_count, "Max threads to use");

    CLI11_PARSE(app, argc, argv);


    std::cout << "<<-- Projector Core -->>" << std::endl;

    projector::environment sim_env;

    std::cout << "Loading input file: " << config_path << std::endl;

    nlohmann::json config_json;
    try {
        config_json = load_json_file(config_path);
    } catch (const std::exception& e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }

    try {
        from_json(config_json, sim_env);
    } catch (const std::exception& e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }

    std::cout << "Loaded config file successfully" << std::endl;


    std::cout << "Loading cross section data from: " << xsdir_path << std::endl;
    try {
        sim_env.cross_section_data = projector::data_library::load_ace_data(xsdir_path);
    } catch (const std::exception& e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }
    std::cout << "Loaded cross section data successfully" << std::endl;

    return EXIT_SUCCESS;
}