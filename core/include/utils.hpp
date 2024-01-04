#pragma once
#include "geometry.hpp"

#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>

std::size_t symbol_to_atomic_number(const std::string_view &symbol);

// utility function for pretty printing nested exceptions
void print_nested_exception(const std::exception &e, int level = 0);

// json loading utility
nlohmann::json load_json_file(std::filesystem::path);

//eigen vector loading from json
template<typename T>
Eigen::Matrix<T, 1, 3> vector_from_json(nlohmann::json &j) {
    if (!j.is_array()) {
        throw std::runtime_error("3d vector is not JSON array");
    }

    if ((j.size() != 3)) {
        throw std::runtime_error("Wrong amount of arguments for vec3");
    }

    T x, y, z;

    j.at(0).get_to(x);
    j.at(1).get_to(y);
    j.at(2).get_to(z);

    return {x, y, z};
}