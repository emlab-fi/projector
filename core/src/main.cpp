#include <iostream>
#include <string>
#include <omp.h>
#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"
#include "geom.hpp"
#include "loader.hpp"
#include "simulator.hpp"

int main(int argc, char* argv[]) {

/*
    namespace po = boost::program_options;


     po::options_description desc("---Cube Gamma Scan Simulator---");
    desc.add_options()
        ("help,h", "print help message")
        ("thread_count,t", po::value<int>()->default_value(omp_get_max_threads()), "Max threads to use")
        ("input_file,i", po::value<std::string>()->required(), "The input json file");
    po::positional_options_description pd;
    pd.add("input_file", -1);

    auto parsed = po::basic_command_line_parser(argc, argv).options(desc).positional(pd).run();
    po::variables_map vm;
    po::store(parsed, vm);

    if (vm.contains("help")) {
        std::cout << desc << std::endl;
        return EXIT_SUCCESS;
    }
    po::notify(vm); */

    std::cout << "---Cube Gamma Scan Simulator---" << std::endl;
    std::cout << "Loading config:" << vm["input_file"].as<std::string>() << std::endl;
    auto config_json = load_file(vm["input_file"].as<std::string>());
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

    std::cout << "Starting simulation with " << vm["thread_count"].as<int>() << " threads" << std::endl;
    auto data = run_simulation(*sim_env, vm["thread_count"].as<int>());

    bool saved = save_data(std::move(data), *sim_env, config_json->at("output_file").get<std::string>());
    if (!saved) {
        std::cerr << "Couldn't save data" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}