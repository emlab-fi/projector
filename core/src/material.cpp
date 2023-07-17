#include <utility>
#include <string>
#include <cctype>
#include <cmath>
#include "material.hpp"
#include "random_numbers.hpp"
#include "constants.hpp"

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


std::pair<double, double> klein_nishina(double k, uint64_t prng_state) {
    using projector::prng_double;

    //standard sampling using Kahn for k < 3, Koblinger for other
    double k_out = 0.0;
    double mu = 0.0;

    double beta = 1.0 + 2.0 * k;

    if (k < 3.0) {
        double x;
        double t = beta / (beta + 8.0);
        while (true) {
            if (prng_double(prng_state) <= t) {
                double r = prng_double(prng_state) * 2.0;
                x = 1.0 + k * r;
                if (prng_double(prng_state) < 4.0 / x * (1.0 - 1.0 / x)) {
                    k_out = k / x;
                    mu = 1 - r;
                }
            } else {
                x = beta / (1.0 + 2.0 * k * prng_double(prng_state));
                mu = 1.0 + (1.0 - x) / k;
                if (prng_double(prng_state) < 0.5 * (mu * mu + 1.0 / x)) {
                    k_out = k / x;
                }
            }
        }

    } else {
        double g = 1.0 - std::pow(beta, -2);

        double t = prng_double(prng_state) *
            (4.0 / k + 0.5 * g + (1.0 - (1.0 + beta) / (k * k)) * std::log(beta));

        if (t <= 2.0 / k) {
            k_out = k / (1.0 + 2.0 * k * prng_double(prng_state));
        } else if (t <= 4.0 / k) {
            k_out = k * (1.0 + 2.0 * k * prng_double(prng_state)) / beta;
        } else if (t <= 4.0 / k + 0.5 * g) {
            k_out = k * std::sqrt( 1.0 - g * prng_double(prng_state));
        } else {
            k_out = k / std::pow(beta, prng_double(prng_state));
        }

        mu = 1.0 + 1.0 / k - 1.0 / k_out;
    }

    return {k_out, mu};
};

} //annonymous namespace


namespace projector {


double element::get_cross_section(double energy, cross_section xs_type) const {

    std::size_t xs = static_cast<std::size_t>(xs_type);

    return interpolate(xs_data[0], xs_data[xs], energy);
}


double element::get_form_factor(double x, form_factor ff_type) const {

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


std::array<double, 5> element::get_all_cross_sections(double energy) const {

    auto [index, t] = calculate_interpolation_values(energy, xs_data[0]);

    std::array<double, 5> xs;
    xs[0] = energy;

    for (std::size_t i = 1; i < 5; ++i) {
        xs[i] = std::lerp(xs_data[i][index], xs_data[i][index+1], t);
    }

    return xs;
}


double element::rayleigh(double energy, uint64_t& prng_state) const {

    double mu = 0.0;

    double k = energy / constants::electron_mass_ev;

    double x2_max = (constants::electron_mass_ev / constants::planck_c) * k;

    double f_max = get_form_factor(x2_max, form_factor::cumulative_coherent);

    while (true) {
        double f = prng_double(prng_state) * f_max;

        double x2 = interpolate(ff_data[3], ff_data[2], f);

        mu = 1.0 - 2.0 * x2 / x2_max;

        if (prng_double(prng_state) < (0.5 * (1.0 + mu * mu))) {
            break;
        }
    }

    return mu;
}

std::pair<double, double> element::compton(double energy, uint64_t& prng_state) const {

    double k = energy / constants::electron_mass_ev;

    double x_max = (constants::electron_mass_ev / constants::planck_c) * k;
    double f_max = get_form_factor(x_max, form_factor::incoherent);

    double k_out = 0.0;
    double mu = 0.0;

    while (true) {
        auto [k_sample, mu_sample] = klein_nishina(k, prng_state);

        double x = constants::electron_mass_ev / constants::planck_c * k_sample * std::sqrt(0.5 * (1 - mu_sample));

        double f = get_form_factor(x, form_factor::incoherent);

        if (prng_double(prng_state) < (f / f_max)) {
            k_out = k_sample;
            mu = mu_sample;
            break;
        }
    }

    return {k_out * constants::electron_mass_ev, mu};
}


const element& data_library::get_element(std::size_t atomic_number) const {
    return elements[atomic_number - 1];
}


const element& data_library::sample_element(const material_data& material, double sample) const {

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