#include "material.hpp"

#include "constants.hpp"
#include "random_numbers.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <string>
#include <utility>

namespace {


std::string parse_name(const std::string_view &material, std::string_view::iterator &pos) {
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

int parse_amount(const std::string_view &material, std::string_view::iterator &pos) {
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

void normalize_vector(std::vector<double> &vec) {
    double sum = 0.0;
    for (double val : vec) {
        sum += val;
    }
    for (double &item : vec) {
        item /= sum;
    }
}

std::pair<std::size_t, double> calculate_interpolation_values(double x,
                                                              const std::vector<double> &values) {

    auto index_iter = std::lower_bound(values.begin(), values.end(), x);

    double left_val = *index_iter;
    double right_val = *std::next(index_iter);

    std::size_t index = std::distance(values.begin(), index_iter);

    double t = (x - left_val) / (right_val - left_val);

    return {index, t};
}

double interpolate(const std::vector<double> &x_vals, const std::vector<double> &y_vals, double x) {

    if (x <= x_vals.front()) {
        return y_vals.front();
    }

    if (x >= x_vals.back()) {
        return y_vals.back();
    }

    auto [index, t] = calculate_interpolation_values(x, x_vals);

    return std::lerp(y_vals[index], y_vals[index + 1], t);
}

std::pair<double, double> klein_nishina(double k, uint64_t &prng_state) {
    using projector::prng_double;

    // standard sampling using Kahn for k < 3, Koblinger for other
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
                    break;
                }
            } else {
                x = beta / (1.0 + 2.0 * k * prng_double(prng_state));
                mu = 1.0 + (1.0 - x) / k;
                if (prng_double(prng_state) < 0.5 * (mu * mu + 1.0 / x)) {
                    k_out = k / x;
                    break;
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
            k_out = k * std::sqrt(1.0 - g * prng_double(prng_state));
        } else {
            k_out = k / std::pow(beta, prng_double(prng_state));
        }

        mu = 1.0 + 1.0 / k - 1.0 / k_out;
    }

    return {k_out, mu};
};

} // namespace

namespace projector {


double element::get_cross_section(double energy, cross_section xs_type) const {

    std::size_t xs = static_cast<std::size_t>(xs_type);

    return interpolate(xs_data[0], xs_data[xs], energy);
}

double element::get_form_factor(double x, form_factor ff_type) const {

    switch (ff_type) {
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

sampled_xs element::get_all_cross_sections(double energy) const {

    auto [index, t] = calculate_interpolation_values(energy, xs_data[0]);

    sampled_xs xs;
    xs.coherent = std::lerp(xs_data[1][index], xs_data[1][index + 1], t);
    xs.incoherent = std::lerp(xs_data[2][index], xs_data[2][index + 1], t);
    xs.photoelectric = std::lerp(xs_data[3][index], xs_data[3][index + 1], t);
    xs.pair_production = std::lerp(xs_data[4][index], xs_data[4][index + 1], t);

    xs.total = xs.coherent + xs.incoherent + xs.photoelectric + xs.pair_production;

    return xs;
}

double element::rayleigh(double energy, uint64_t &prng_state) const {

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

std::pair<double, double> element::compton(double energy, uint64_t &prng_state) const {

    double k = energy / constants::electron_mass_ev;

    double x_max = (constants::electron_mass_ev / constants::planck_c) * k;
    double f_max = get_form_factor(x_max, form_factor::incoherent);

    double k_out = 0.0;
    double mu = 0.0;

    while (true) {
        auto [k_sample, mu_sample] = klein_nishina(k, prng_state);

        double x = constants::electron_mass_ev / constants::planck_c * k_sample *
                   std::sqrt(0.5 * (1 - mu_sample));

        double f = get_form_factor(x, form_factor::incoherent);

        if (prng_double(prng_state) < (f / f_max)) {
            k_out = k_sample;
            mu = mu_sample;
            break;
        }
    }

    return {k_out * constants::electron_mass_ev, mu};
}

double data_library::material_macro_xs(const material_data &mat, double energy) const {

    double total_xs = 0.0;

    for (std::size_t i = 0; i < mat.elements.size(); ++i) {
        double atomic_density = mat.atom_density[i];
        sampled_xs elem_xs = get_element(mat.elements[i]).get_all_cross_sections(energy);
        total_xs += atomic_density * elem_xs.total;
    }

    return total_xs;
};

const element &data_library::get_element(std::size_t atomic_number) const {
    return elements[atomic_number - 1];
}

std::pair<double, const element &> data_library::sample_material(const material_data &material,
                                                                 double energy,
                                                                 uint64_t &prng_state) const {

    // calculate total material macro xs
    std::vector<double> total_xs_cumulative;

    double temp = 0.0;
    for (std::size_t i = 0; i < material.elements.size(); ++i) {
        double elem_xs = get_element(material.elements[i]).get_all_cross_sections(energy).total;

        temp += elem_xs * material.atom_density[i];
        total_xs_cumulative.push_back(temp);
    }

    // sample an element
    double sample = prng_double(prng_state) * total_xs_cumulative.back();

    for (std::size_t i = 0; i < total_xs_cumulative.size(); ++i) {
        if (sample < total_xs_cumulative[i]) {
            return {total_xs_cumulative.back(), get_element(material.elements[i])};
            break;
        }
    }

    throw std::runtime_error("Couldn't sample element");
}

void data_library::material_calculate_missing_values(material_data &mat) const {

    if (mat.atomic_percentage.size() != 0) {

        normalize_vector(mat.atomic_percentage);

        double total = 0.0;

        for (std::size_t i = 0; i < mat.elements.size(); ++i) {
            total += mat.atomic_percentage[i] * get_element(mat.elements[i]).atomic_weight;
        }

        for (std::size_t i = 0; i < mat.elements.size(); ++i) {
            double weight_percent =
                (mat.atomic_percentage[i] * get_element(mat.elements[i]).atomic_weight) / total;
            mat.weight_percentage.push_back(weight_percent);
        }

    }

    else if (mat.weight_percentage.size() != 0) {

        normalize_vector(mat.weight_percentage);

        double total = 0.0;

        for (std::size_t i = 0; i < mat.elements.size(); ++i) {
            total += mat.weight_percentage[i] / get_element(mat.elements[i]).atomic_weight;
        }

        for (std::size_t i = 0; i < mat.elements.size(); ++i) {
            double atom_percent =
                (mat.weight_percentage[i] / get_element(mat.elements[i]).atomic_weight) / total;
            mat.atomic_percentage.push_back(atom_percent);
        }
    }

    double min_value =
        *std::min_element(mat.atomic_percentage.begin(), mat.atomic_percentage.end());

    mat.molar_mass = 0.0;

    double atoms_count = 0.0;

    for (std::size_t i = 0; i < mat.elements.size(); ++i) {
        double atom_count = mat.atomic_percentage[i] / min_value;
        atoms_count += atom_count;
        mat.molar_mass += atom_count * get_element(mat.elements[i]).atomic_weight;
    }

    mat.total_atomic_density = (atoms_count * mat.density * constants::avogadro) / mat.molar_mass;

    for (std::size_t i = 0; i < mat.elements.size(); ++i) {
        double atom_density = mat.total_atomic_density * mat.atomic_percentage[i];
        mat.atom_density.push_back(atom_density);
    }
}

parsed_material parse_material_string(const std::string_view &material) {

    parsed_material parsed;

    auto pos = material.begin();

    while (pos != material.end()) {
        std::string parsed_name = parse_name(material, pos);
        int amount = parse_amount(material, pos);

        parsed.emplace_back(parsed_name, amount);
    }

    return parsed;
}


} // namespace projector