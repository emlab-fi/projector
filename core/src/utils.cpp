#include <iostream>
#include <fstream>
#include <variant>
#include "utils.hpp"

namespace {

constexpr std::string_view op_type_map[] = {
    "nop", "join", "intersect", "cut"
};

constexpr std::string_view shape_map[] = {
    "plane", "aa_box", "aa_ellipsoid", "aa_cylinder"
};

} //annonymous namespace


void print_nested_exception(const std::exception& e, int level) {
    std::cerr << std::string(level * 2, ' ') << "Exception: " << e.what() << std::endl;
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception& next_exception) {
        print_nested_exception(next_exception, level + 1);
    } catch (...) {}
}


void print_geometry(const projector::geometry& geom, int level) {

    std::string prefix = std::string(level * 2, ' ');
    std::cout << prefix << "geometry:" << std::endl;

    std::visit([level](auto&& arg) {

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, projector::geom_primitive>) {
            print_primitive(arg, level + 1);
        } else if constexpr (std::is_same_v<T, projector::operation>) {
            print_operation(arg, level + 1);
        }

    }, geom.definition);
}


void print_primitive(const projector::geom_primitive& prim, int level) {
    std::string prefix = std::string(level * 2, ' ');

    std::cout << prefix << "primitive: " << shape_map[static_cast<int>(prim.type)] << std::endl;
    std::cout << prefix << "param1: " << prim.param1.transpose() << std::endl;
    std::cout << prefix << "param2: " << prim.param2.transpose() << std::endl;
}


void print_operation(const projector::operation& op, int level) {
    std::string prefix = std::string(level * 2, ' ');

    std::cout << prefix << "operation: " << op_type_map[static_cast<int>(op.op)] << std::endl;

    std::cout << prefix << "left:" << std::endl;
    print_geometry(*op.left, level + 1);

    std::cout << prefix << "right:" << std::endl;
    print_geometry(*op.right, level + 1);
}

nlohmann::json load_json_file(std::filesystem::path filepath) {
    std::ifstream input_file(filepath);

    if (!input_file.is_open()) {
        throw std::runtime_error("Can't open file: " + filepath.string());
    }

    nlohmann::json parsed;

    try {
        parsed = nlohmann::json::parse(input_file, nullptr, false, false);
    } catch (std::exception& e) {
        std::throw_with_nested("Can't parse JSON");
    }

    input_file.close();

    return parsed;
}