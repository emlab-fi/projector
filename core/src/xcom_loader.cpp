#include <fstream>
#include <cstdio>
#include <stdexcept>
#include "cross_sections.hpp"


namespace {

void ignore_read(std::ifstream& stream, std::size_t count) {
    std::string temp;
    for (std::size_t i = 0; i < count; ++i) {
        stream >> temp;
    }
}

bool read_file_header(std::ifstream& stream, uint32_t& atomic_number, double& atomic_weight, std::size_t& energy_count) {
    std::size_t edge_count;

    stream >> atomic_number;
    stream >> atomic_weight;
    stream >> edge_count;
    stream >> energy_count;

    //ignore the absorption edge data
    ignore_read(stream, 3 * edge_count);

    return stream.good();
}

bool read_data_segment(std::ifstream& stream, std::vector<double>& data, std::size_t count) {
    data = std::vector<double>(count);

    for (std::size_t i = 0; i < count; ++i) {
        stream >> data[i];
    }

    return stream.good();
}

} //anonymous namespace

namespace projector {


element_entry element_entry::load_xcom_file(std::filesystem::path file) {

    auto filestream = std::ifstream(file);
    if (!filestream.is_open()) {
        throw std::runtime_error("Can't open file");
    }

    projector::element_entry data;

    if (!read_file_header(filestream, data.atomic_number, data.atomic_weight, data.energy_count)) {
        throw std::runtime_error("Can't read data header");
    }

    for (std::size_t i; i < 6; ++i) {
        if (!read_data_segment(filestream, data.xs_data[i], data.energy_count)) {
            throw std::runtime_error("Can't read data segments");
        }
    }

    for (std::size_t i = 0; i < data.energy_count; ++i) {
        double total = 0.0;
        for (std::size_t s = 0; i < 6; ++i) {
            total += data.xs_data[s][i];
        }
        data.xs_data[6][i] = total;
    }

    filestream.close();

    return data;
}


data_library data_library::load_xcom_data(std::filesystem::path path) {

    constexpr char* fmt_string = "MDATX3.%03zu";

    data_library library;

    for (std::size_t i = 1; i <= 100; ++i) {

        char file_path[11] = {0};
        std::sprintf(file_path, fmt_string, i);

        try {
            auto element_data = element_entry::load_xcom_file(path / file_path);
            library.elements[i - 1] = element_data;
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Error while reading XCOM file: " + (path/file_path).string()));
        }
    }

    return library;
}

} //namespace projector