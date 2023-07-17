#pragma once
#include <string_view>
#include <optional>
#include <cstdint>
#include <array>
#include <vector>
#include <filesystem>


namespace projector {

enum class cross_section {
    no_interaction = 0,
    coherent = 1,
    incoherent = 2,
    photoelectric = 3,
    pair_production = 4
};

enum class form_factor {
    no_form_factor = 0,
    incoherent = 1,
    differential_coherent = 2,
    cumulative_coherent = 3
};

//vector containing normalized material data, pair for each present element
//    first - atomic number
//    second - percentage, normalized to 0 - 1
using material_data = std::vector<std::pair<std::size_t, double>>;

//parsed chemical formula, pair of symbol and count
using parsed_material = std::vector<std::pair<std::string_view, int>>;


class element {

    std::array<std::vector<double>, 5> xs_data;
    std::array<std::vector<double>, 5> ff_data;

public:

    uint32_t atomic_number;
    double atomic_weight;

    double get_cross_section(double energy, cross_section xs_type) const;

    double get_form_factor(double x, form_factor ff_type) const;

    std::array<double, 5> get_all_cross_sections(double energy) const;

    double rayleigh(double energy, uint64_t& prng_state) const;

    // deprecated! use endf
    static element load_xcom_file(std::filesystem::path path);

    static element load_from_ace_file(std::filesystem::path path, std::size_t line);

};


class data_library {

    std::array<element, 100> elements;

public:

    const element& get_element(std::size_t atomic_number) const;

    const element& sample_element(const material_data& elements, double sample) const;

    material_data preprocess_cross_sections(const parsed_material& input_data) const;

    // deprecated! use endf
    static data_library load_xcom_data(std::filesystem::path path);

    static data_library load_ace_data(std::filesystem::path path);

};


parsed_material parse_material_string(const std::string_view& material);



} //namespace projector