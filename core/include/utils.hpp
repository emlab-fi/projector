#pragma once
#include "geometry.hpp"

#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>

std::size_t symbol_to_atomic_number(const std::string_view &symbol);

// utility function for pretty printing nested exceptions
void print_nested_exception(const std::exception &e, int level = 0);

// pretty printing for geometry
void print_geometry(const projector::geometry &geom, int level = 0);
void print_primitive(const projector::geom_primitive &prim, int level = 0);
void print_operation(const projector::operation &op, int level = 0);

// json loading utility
nlohmann::json load_json_file(std::filesystem::path);