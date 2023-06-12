#include <fstream>
#include <filesystem>
#include <cstdio>
#include "cross_sections.hpp"


namespace {

void ignore_read(std::ifstream& stream, std::size_t count) {
    std::string temp;
    for (std::size_t i = 0; i < count; ++i) {
        stream >> temp;
    }
}

bool read_file_header(std::ifstream& stream, projector::element_entry& data) {
    std::size_t edge_count;

    stream >> data.atomic_number;
    stream >> data.atomic_weight;
    stream >> edge_count;
    stream >> data.energy_count;

    //ignore the absorbtion edge data
    ignore_read(stream, 3 * edge_count);

    return stream.good();
}

bool read_data_segment(std::ifstream& stream, projector::element_entry& data, std::size_t segment) {
    data.data[segment] = std::vector<double>(data.energy_count);

    for (std::size_t i = 0; i < data.energy_count; ++i) {
        stream >> data.data[segment][i];
    }

    return stream.good();
}

std::optional<projector::element_entry> load_file(std::filesystem::path file) {

    auto filestream = std::ifstream(file);
    if (!filestream.is_open()) {
        return {};
    }

    projector::element_entry data;

    if (!read_file_header(filestream, data)) {
        return {};
    }

    for (std::size_t i; i < 6; ++i) {
        if (!read_data_segment(filestream, data, i)) {
            return {};
        }
    }

    filestream.close();

    return data;
}

} //anonymous namespace

namespace projector {

std::optional<data_library> load_xcom_data(std::string_view path) {
    auto xcom_path = std::filesystem::path(path);

    constexpr char* fmt_string = "MDATX3.%03zu";

    data_library library;

    for (std::size_t i = 1; i <= 100; ++i) {

        char file_path[11] = {0};
        std::sprintf(file_path, fmt_string, i);

        auto element_data = load_file(xcom_path / file_path);

        if (!element_data) {
            return {};
        }

        library.elements[i - 1] = *element_data;
    }

    return library;
}

} //namespace projector