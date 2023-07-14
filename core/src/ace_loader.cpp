#include "cross_sections.hpp"
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include <cmath>


// This currently expects the exact format of eprdata14 files
// It probably won't work on other data


namespace {

void ignore_lines(std::ifstream& file_stream, std::size_t lines) {
    char temp[81];

    for (std::size_t i = 0; i < lines; ++i) {
        file_stream.getline(temp, 81);
    }
}

template<typename T>
void read_to_array(std::ifstream& src, T& dest, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
        src >> dest[i];
    }
}

//this should have a concept!!
template<typename F>
void read_map_insert(std::ifstream& src, std::vector<double>& dest, std::size_t count, F map_func) {
    for (std::size_t i = 0; i < count; ++i) {
        double val;
        src >> val;
        dest.push_back(map_func(val));
    }
}

} // annonymous namespace


namespace projector {

element_entry element_entry::load_from_ace_file(std::filesystem::path input_file, std::size_t line) {
    auto file_stream = std::ifstream(input_file);
    if (!file_stream.is_open()) {
        throw std::runtime_error("Failed opening " + input_file.string());
    }

    //seek to line with first interesting data - first line of NXS block
    ignore_lines(file_stream, line + 6);
    if (!file_stream.good()) {
        throw std::runtime_error("Error while seeking");
    }

    std::string temp;

    std::array<std::size_t, 16> NXS;
    std::array<std::size_t, 32> JXS;

    read_to_array(file_stream, NXS, 16);
    read_to_array(file_stream, JXS, 32);

    if (!file_stream.good()) {
        throw std::runtime_error("Error while reading NXS/JXS data");
    }

    std::size_t N_y = NXS[2];
    std::size_t N_inc = (JXS[2] - JXS[1]) / 2;
    std::size_t N_coh = (JXS[3] - JXS[2]) / 3;

    element_entry elem;
    elem.atomic_number = NXS[1];

    auto unlog = [](double val){
        if (val == 0.0) {
            return 0.0;
        }
        return std::exp(val);
    };

    auto no_op = [](double val){return val;};

    try {
        for (std::size_t i = 0; i < 5; ++i) {
            read_map_insert(file_stream, elem.xs_data[i], N_y, unlog);
        }
        for (std::size_t i = 0; i < 2; ++i) {
            read_map_insert(file_stream, elem.ff_data[i], N_inc, no_op);
        }
        for (std::size_t i = 0; i < 3; ++i) {
            read_map_insert(file_stream, elem.ff_data[i+2], N_coh, no_op);
        }
    } catch (...) {
        std::throw_with_nested("Error while reading main data block");
    }

    return elem;
}


data_library data_library::load_ace_data(std::filesystem::path xsdir_file) {

    auto xsdir_stream = std::ifstream(xsdir_file);
    if (!xsdir_stream.is_open()) {
        throw std::runtime_error("Failed opening " + xsdir_file.string());
    }

    data_library data;

    for (std::size_t i = 0; i < 100; ++i) {
        std::string line;
        std::getline(xsdir_stream, line);

        if (!xsdir_stream.good()) {
            throw std::runtime_error("Error while opening xsdir file");
        }

        char ace_path_str[256];
        double weight;
        std::size_t start_line;

        int read = sscanf(line.c_str(), "%*ld.14p %lf %s 0 1 %zu %*ld %*ld %*ld %*lf", &weight, &ace_path_str, &start_line);
        if (read != 3) {
            throw std::runtime_error("Error while parsing xsdir line: " + line);
        }


        element_entry elem;
        std::filesystem::path ace_path(ace_path_str);
        ace_path = xsdir_file.parent_path() / ace_path;

        try {
            elem = element_entry::load_from_ace_file(ace_path, start_line - 1);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Error while reading ACE file: " + ace_path.string()));
        }


        elem.atomic_number = i + 1;
        elem.atomic_weight = weight;

        data.elements[i] = elem;

    }

    xsdir_stream.close();

    return data;
}

} // namespace projector