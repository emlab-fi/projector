#include <utility>
#include <string>
#include <cctype>
#include "cross_sections.hpp"

namespace {

std::size_t symbol_to_index(const std::string_view& symbol) {
    constexpr std::array<std::string_view, 100> symbols = {
        "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne", "Na", "Mg", "Al",
        "Si", "P", "S", "Cl", "Ar", "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe",
        "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr",
        "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",
        "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm",
        "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", "W",
        "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn",
        "Fr", "Ra", "Ac", "Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf",
        "Es", "Fm"
    };

    for (std::size_t i = 0; i < 100; ++i) {
        if (symbols[i] == symbol) {
            return i;
        }
    }

    return 0;
}

std::string parse_name(const std::string_view& material, std::string_view::iterator& pos) {
    if (!std::isupper(*pos) && pos != material.end()) {
        return "";
    }

    std::string out;

    out += *pos;

    ++pos;

    if (std::islower(*pos) && pos != material.end()) {
        out += *pos;
        ++pos;
    }

    return out;
}

int parse_amount(const std::string_view& material, std::string_view::iterator& pos) {
    if (pos == material.end()) {
        return 1;
    }

    if (!std::isdigit(*pos) && pos != material.end()) {
        return 1;
    }

    std::string read;

    while (std::isdigit(*pos) && pos != material.end()) {
        read += *pos;
        ++pos;
    }

    return std::stoi(read);
}

} //annonymous namespace


namespace projector {

std::pair<int, double> element_entry::calculate_interpolation_values(double energy) const {
    if (energy <= xs_data[0][0]) {
        return {0, 0.0};
    }

    if (energy >= xs_data[0][energy_count - 1]) {
        return {energy_count - 1, 1.0};
    }

    int index = 0;

    while (xs_data[0][index] < energy) {
        ++index;
    }

    double t = (energy - xs_data[0][index]) /  (xs_data[0][index + 1] - xs_data[0][index]);

    return {index, t};
}


double element_entry::retrieve_cross_section(double t, std::size_t index, cross_section xs_type) const {
    return (1.0 - t) * xs_data[static_cast<int>(xs_type)][index] + t * xs_data[static_cast<int>(xs_type)][index + 1];
}


double element_entry::get_cross_section(double energy, cross_section xs_type) const {
    auto [index, t] = calculate_interpolation_values(energy);
    return retrieve_cross_section(t, index, xs_type);
}


material_data data_library::preprocess_cross_sections(const parsed_material& input_data) const {

    double total_weight = 0.0;

    material_data converted{};

    for (const auto& [element_symbol, count] : input_data) {
        std::size_t index = symbol_to_index(element_symbol);
        double weight = elements[index].atomic_weight * count;
        total_weight += weight;

        converted.emplace_back(index + 1, weight);
    }

    for (auto& item : converted) {
        item.second /= total_weight;
    }

    return converted;
}

parsed_material parse_material_string(const std::string_view& material) {

    parsed_material parsed;

    auto pos = material.begin();

    while (pos != material.end()) {
        std::string parsed_name = parse_name(material, pos);
        int amount = parse_amount(material, pos);

        parsed.emplace_back(parsed_name, amount);
    }

    return parsed;
}


} //namespace projector