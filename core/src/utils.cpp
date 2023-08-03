#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <variant>

std::size_t symbol_to_atomic_number(const std::string_view &symbol) {
    constexpr std::array<std::string_view, 100> symbols = {
        "H",  "He", "Li", "Be", "B",  "C",  "N",  "O",  "F",  "Ne", "Na", "Mg",
        "Al", "Si", "P",  "S",  "Cl", "Ar", "K",  "Ca", "Sc", "Ti", "V",  "Cr",
        "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr",
        "Rb", "Sr", "Y",  "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd",
        "In", "Sn", "Sb", "Te", "I",  "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",
        "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf",
        "Ta", "W",  "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po",
        "At", "Rn", "Fr", "Ra", "Ac", "Th", "Pa", "U",  "Np", "Pu", "Am", "Cm",
        "Bk", "Cf", "Es", "Fm"};

    for (std::size_t i = 0; i < 100; ++i) {
        if (symbols[i] == symbol) {
            return i + 1;
        }
    }

    return 0;
}

void print_nested_exception(const std::exception &e, int level) {
    std::cerr << std::string(level * 2, ' ') << "Exception: " << e.what()
              << std::endl;
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception &next_exception) {
        print_nested_exception(next_exception, level + 1);
    } catch (...) {
    }
}

nlohmann::json load_json_file(std::filesystem::path filepath) {
    std::ifstream input_file(filepath);

    if (!input_file.is_open()) {
        throw std::runtime_error("Can't open file: " + filepath.string());
    }

    nlohmann::json parsed;

    try {
        parsed = nlohmann::json::parse(input_file, nullptr, false, false);
    } catch (std::exception &e) {
        std::throw_with_nested("Can't parse JSON");
    }

    input_file.close();

    return parsed;
}