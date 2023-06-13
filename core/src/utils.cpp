#include <iostream>
#include "utils.hpp"


void print_nested_exception(const std::exception& e, int level) {
    std::cerr << std::string(level * 2, ' ') << "Exception: " << e.what() << std::endl;
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception& next_exception) {
        print_nested_exception(next_exception, level + 1);
    } catch (...) {}
}