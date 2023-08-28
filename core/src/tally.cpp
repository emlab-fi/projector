#include "tally.hpp"

#include <fstream>
#include <type_traits>

namespace projector {

std::optional<coord3> uniform_mesh_tally::determine_cell(const vec3 &point) const {
    if (!bounds.point_inside(point)) {
        return {};
    }

    coord3 output = {0, 0, 0};

    for (std::size_t i = 0; i < 3; ++i) {
        double shifted = point[i] - bounds.min[i];
        double step_size = std::abs(bounds.max[i] - bounds.min[i]) / resolution[i];

        output[i] = std::floor(shifted / step_size);
    }

    return output;
}

std::size_t uniform_mesh_tally::calculate_index(const coord3 &c) const {
    std::size_t i = c.z() * (resolution.x() * resolution.y() * stride) +
                    c.y() * resolution.x() * stride + c.x() * stride;

    return i;
}

uniform_mesh_tally::uniform_mesh_tally(const vec3 &start, const vec3 &end, const coord3 &res,
                                       tally_score sc)
    : resolution(res), score(sc) {

    bounds.min = start;
    bounds.max = end;

    switch (score) {
    case tally_score::flux:
    case tally_score::average_energy:
        stride = 1;
        break;
    case tally_score::interaction_counts:
        stride = 5;
        break;
    default:
        throw std::runtime_error("unsupported score for uniform mesh tally");
        break;
    }
}

void uniform_mesh_tally::init_tally() {

    std::size_t total = resolution.x() * resolution.y() * resolution.z() * stride;

    // init to proper type depending on score
    switch (score) {
    case tally_score::average_energy:
        data.resize(total, double(0.0));
        break;
    case tally_score::flux:
    case tally_score::interaction_counts:
        data.resize(total, int(0));
        break;
    }
}

void uniform_mesh_tally::add_particle(const particle &p) {}

void uniform_mesh_tally::finalize_data() {
    // empty implementation
}

void uniform_mesh_tally::save_tally(const std::filesystem::path path) const {

    std::fstream output_file(path, output_file.trunc | output_file.out);

    if (!output_file.is_open()) {
        throw std::runtime_error("failed to open file: " + path.string());
    }

    output_file << "x,y,z";
    for (std::size_t i = 0; i < stride; ++i) {
        output_file << ",data" << i;
    }
    output_file << "\n";

    output_file << std::setprecision(10) << std::scientific;

    auto print_variant = [&output_file](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_arithmetic_v<T>) {
            output_file << "," << arg;
        }
    };

    for (std::size_t z = 0; z < resolution.z(); ++z) {
        for (std::size_t y = 0; y < resolution.y(); ++z) {
            for (std::size_t x = 0; x < resolution.x(); ++x) {

                output_file << x << "," << y << "," << z;

                std::size_t base = calculate_index({x,y,z});

                for (std::size_t i = 0; i < stride; ++i) {
                    //THIS DOESNT WORK BECAUSE VARIANT
                    print_variant(data[base + i]);
                }

                output_file << "\n";
            }
        }
    }
}

} // namespace projector