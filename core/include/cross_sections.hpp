#pragma once
#include <string_view>
#include <optional>
#include <cstdint>
#include <memory>
#include <array>
#include <optional>

namespace projector {

enum class cross_section : std::size_t {
    coherent = 1,
    incoherent = 2,
    photoelectric = 3,
    pair_nuclear = 4,
    pair_electron = 5
};



struct element_entry {

uint32_t atomic_number;
double atomic_weight;

std::size_t energy_counts;

std::array<std::unique_ptr<double*>, 6> data;

double retrieve_cross_section(double energy, cross_section xs_type);

};



class data_library {

std::array<element_entry, 100> elements;

friend std::optional<data_library> load_xcom_data(std::string_view path);

public:



};


std::optional<data_library> load_xcom_data(std::string_view path);

} //namespace projector