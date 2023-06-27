#pragma once
#include <string_view>
#include <optional>
#include <cstdint>
#include <array>
#include <vector>
namespace projector {

enum class cross_section {
    no_interaction = 0,
    coherent = 1,
    incoherent = 2,
    photoelectric = 3,
    pair_nuclear = 4,
    pair_electron = 5
};


/*
vector containing normalized material data, pair for each present element
    first - atomic number
    second - percentage, normalized to 0 - 1
*/
using material_data = std::vector<std::pair<std::size_t, double>>;

using parsed_material = std::vector<std::pair<std::string_view, int>>;

using sampled_cross_sections = std::array<double, 6>;


struct element_entry {

    uint32_t atomic_number;
    double atomic_weight;

    std::size_t energy_count;

    std::array<std::vector<double>, 6> data;


    std::pair<int, double> calculate_interpolation_values(double energy) const;

    double interpolate_cross_section(double energy, cross_section xs_type) const;

    double retrieve_cross_section(double t, std::size_t index, cross_section xs_type) const;

};


class data_library {

    std::array<element_entry, 100> elements;

    friend data_library load_xcom_data(std::string_view path);

public:

    sampled_cros_sections sample_cross_sections(const material_data& elements, double energy, double sample) const;

    material_data preprocess_cross_sections(const parsed_material& input_data) const;

};


data_library load_xcom_data(std::string_view path);

parsed_material parse_material_string(std::string_view material);



} //namespace projector