#include <iostream>
#include <string_view>
#include <nlohmann/json.hpp>
#include "CLI/CLI.hpp"
#include "cross_sections.hpp"
#include "utils.hpp"
#include "geometry.hpp"
#include "environment.hpp"
#include "json_converters.hpp"


int main(int argc, char* argv[]) {

    CLI::App app{"Simple particle simulator"};

    std::string_view config_path;
    std::string_view xcom_path;
    int thread_count = 1;

    app.add_option("--xcom, -x, xcom_path", xcom_path, "XCOM database path")->required()->check(CLI::ExistingPath);
    app.add_option("--input_file, -i, input_file", config_path, "Input json file")->required()->check(CLI::ExistingFile);
    app.add_option("--thread_count, -t", thread_count, "Max threads to use");

    CLI11_PARSE(app, argc, argv);


    std::cout << "<<-- Projector Core -->>" << std::endl;

    std::cout << "Loading XCOM data from: " << xcom_path << std::endl;

    projector::data_library XCOM_database;
    try {
        XCOM_database = projector::load_xcom_data(xcom_path);
    } catch (const std::exception& e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }
    std::cout << "Loaded XCOM data successfully" << std::endl;



    std::cout << "Loading input file: " << config_path << std::endl;

    nlohmann::json config_json;
    try {
        config_json = load_json_file(config_path);
    } catch (const std::exception& e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }

    projector::environment sim_env;
    try {
        from_json(config_json, sim_env);
    } catch (const std::exception& e) {
        print_nested_exception(e);
        return EXIT_FAILURE;
    }

    std::cout << "Loaded config file successfully" << std::endl;

    for (auto& obj : sim_env.objects) {
        std::cout <<"<==" << obj.id << "==>" << std::endl;
        print_geometry(obj.geom);
    }

    return EXIT_SUCCESS;
}