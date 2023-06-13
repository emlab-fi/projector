#include <utility>
#include "cross_sections.hpp"

namespace projector {

std::pair<int, double> element_entry::calculate_interpolation_values(double energy) const {
    if (energy <= data[0][0]) {
        return {0, 0.0};
    }

    if (energy >= data[0][energy_count - 1]) {
        return {energy_count - 1, 1.0};
    }

    int index = 0;

    while (data[0][index] < energy) {
        ++index;
    }

    double t = (energy - data[0][index]) /  (data[0][index + 1] - data[0][index]);

    return {index, t};
}


double element_entry::retrieve_cross_section(double t, std::size_t index, cross_section xs_type) const {
    return (1.0 - t) * data[static_cast<int>(xs_type)][index] + t * data[static_cast<int>(xs_type)][index + 1];
}


double element_entry::interpolate_cross_section(double energy, cross_section xs_type) const {
    auto [index, t] = calculate_interpolation_values(energy);
    return retrieve_cross_section(t, index, xs_type);
}


std::array<double, 6> data_library::sample_cross_sections(const material_data& material, double energy, double sample) const {
    std::size_t selected_element;
    double curr_percentage = 0.0;

    for (auto& [element, percentage] : material) {
        curr_percentage += percentage;
        selected_element = element - 1;
        if (curr_percentage <= sample) {
            break;
        }
    }

    const element_entry& element = elements[selected_element];

    auto [index, t] = element.calculate_interpolation_values(energy);

    std::array<double, 6> data = {
        energy,
        element.retrieve_cross_section(t, index, cross_section::coherent),
        element.retrieve_cross_section(t, index, cross_section::incoherent),
        element.retrieve_cross_section(t, index, cross_section::photoelectric),
        element.retrieve_cross_section(t, index, cross_section::pair_nuclear),
        element.retrieve_cross_section(t, index, cross_section::pair_electron)
    };

    return data;
}


material_data preprocess_cross_sections(const std::vector<std::pair<std::string_view, int>>& input_data) {

}


} //namespace projector