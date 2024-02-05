#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <termcolor/termcolor.hpp>
#include <variant>

std::size_t symbol_to_atomic_number(const std::string_view &symbol) {
    constexpr std::array<std::string_view, 100> symbols = {
        "H",  "He", "Li", "Be", "B",  "C",  "N",  "O",  "F",  "Ne", "Na", "Mg", "Al", "Si", "P",
        "S",  "Cl", "Ar", "K",  "Ca", "Sc", "Ti", "V",  "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
        "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y",  "Zr", "Nb", "Mo", "Tc", "Ru", "Rh",
        "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I",  "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",
        "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", "W",  "Re",
        "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
        "Pa", "U",  "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm"};

    for (std::size_t i = 0; i < 100; ++i) {
        if (symbols[i] == symbol) {
            return i + 1;
        }
    }

    return 0;
}

void print_material(const projector::material_data &mat, const std::string &name) {
    std::cout << termcolor::white << termcolor::bold << "-----" << name << "-----"
              << termcolor::reset << std::endl;
    std::cout << "density: " << mat.density << std::endl;
    std::cout << "molar mass: " << mat.molar_mass << std::endl;
    std::cout << "atomic density: " << mat.total_atomic_density << std::endl;
    std::cout << "atomic density (atoms/b^cm): " << mat.total_atomic_density * 1e-24 << std::endl;
    std::cout << termcolor::white << termcolor::bold
              << "Element     Weight %     Atomic %        Atomic density" << termcolor::reset
              << std::endl;
    for (std::size_t j = 0; j < mat.elements.size(); ++j) {
        std::cout << std::setw(7) << mat.elements[j] << " ";
        std::cout << std::setw(12) << mat.weight_percentage[j] << " ";
        std::cout << std::setw(12) << mat.atomic_percentage[j] << " ";
        std::cout << std::setw(12) << mat.atom_density[j] << " ";
        std::cout << std::setw(12) << mat.atom_density[j] * 1e-24 << " " << std::endl;
    }
}

void print_nested_exception(const std::exception &e, int level) {
    std::cerr << std::string(level * 2, ' ') << "Exception: " << e.what() << std::endl;
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
        parsed = nlohmann::json::parse(input_file, nullptr, false, true);
    } catch (std::exception &e) {
        std::throw_with_nested("Can't parse JSON");
    }

    input_file.close();

    return parsed;
}