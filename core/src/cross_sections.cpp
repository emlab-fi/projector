#include <utility>
#include <string>
#include <cctype>
#include <cmath>
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

std::pair<std::size_t, double> calculate_interpolation_values(double x, const std::vector<double>& values) {
    std::size_t index = 0;

    //currently a simple linear search, will perform badly!
    while (values[index] > x) {
        ++index;
    }

    double t = (x - values[index]) / (values[index+1] - values[index]);

    return {index, t};
}

double interpolate(const std::vector<double>& x_vals, const std::vector<double>& y_vals, double x) {

    if (x <= x_vals.front()) {
        return y_vals.front();
    }

    if (x >= x_vals.back()) {
        return y_vals.back();
    }

    auto [index, t] = calculate_interpolation_values(x, x_vals);

    return std::lerp(y_vals[index], y_vals[index+1], t);
}

} //annonymous namespace


namespace projector {


double element_entry::get_cross_section(double energy, cross_section xs_type) const {

    std::size_t xs = static_cast<std::size_t>(xs_type);

    return interpolate(xs_data[0], xs_data[xs], energy);
}


double element_entry::get_form_factor(double x, form_factor ff_type) const {

    switch(ff_type) {
    case form_factor::incoherent:
        return interpolate(ff_data[0], ff_data[1], x);
    case form_factor::cumulative_coherent:
        return interpolate(ff_data[2], ff_data[3], x);
    case form_factor::differential_coherent:
        return interpolate(ff_data[2], ff_data[4], x);
    default:
        throw std::runtime_error("invalid form factor request!");
    }

    return 0.0;
}


const element_entry& data_library::get_element(std::size_t atomic_number) const {
    return elements[atomic_number - 1];
}


const element_entry& data_library::sample_element(const material_data& material, double sample) const {

    double cumulative = 0.0;

    for (auto [atomic_number, amount] : material) {
        cumulative += amount;
        if (cumulative >= sample) {
            return get_element(atomic_number);
        }
    }

    throw std::runtime_error("Couldn't sample element");
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