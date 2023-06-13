#include <iostream>
#include <string_view>
#include "CLI/CLI.hpp"
#include "cross_sections.hpp"
#include "utils.hpp"

int main(int argc, char* argv[]) {

    CLI::App app{"Simple particle simulator"};

    std::string_view config_path;
    std::string_view xcom_path;
    int thread_count = 1;

    app.add_option("--xcom, -x, xcom_path", xcom_path, "XCOM database path")->required()->check(CLI::ExistingPath);
    //app.add_option("--input_file, -i, input_file", config_path, "Input json file")->required()->check(CLI::ExistingFile);
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


    return EXIT_SUCCESS;
}