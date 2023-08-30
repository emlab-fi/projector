#include "tally.hpp"

#include "constants.hpp"

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

std::vector<double> uniform_mesh_tally::calculate_intersections(const vec3 &start,
                                                                const vec3 &end) const {

    return {};
}

void uniform_mesh_tally::increment_index(std::size_t index) {

    auto increment_visit = [](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_arithmetic_v<T>) {
            arg += 1;
        }
    };

    std::visit(increment_visit, data[index]);
}

void uniform_mesh_tally::update_mean_index(std::size_t index, double value) {
    
    auto add_visit = [&value](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_arithmetic_v<T>) {
            arg += value;
        }
    };

    increment_index(index + 1);

    std::visit(add_visit, data[index]);
}

void uniform_mesh_tally::add_particle_interactionwise(const particle &p) {
    std::size_t amount = p.history.points.size();

    for (std::size_t i = 0; i < amount; ++i) {

        auto coord = determine_cell(p.history.points[i]);

        if (coord && p.history.interactions[i] != cross_section::no_interaction) {
            std::size_t data_index = calculate_index(*coord);

            increment_index(data_index);

            data_index += static_cast<std::size_t>(p.history.interactions[i]);

            increment_index(data_index);
        }
    }
}

void uniform_mesh_tally::add_particle_segmentwise(const particle &p) {

    std::size_t amount = p.history.points.size() - 1;

    for (std::size_t i = 0; i < amount; ++i) {

        const vec3 &start = p.history.points[i];
        const vec3 &end = p.history.points[i + 1];

        if (!bounds.segment_intersect(start, end)) {
            continue;
        }

        std::vector<double> intersects = calculate_intersections(start, end);

        for (double &t : intersects) {
            // get a point a tiny bit behind the intersection
            vec3 point = start + (t + 5 * constants::epsilon) * (end - start);

            auto coordinates = determine_cell(point);

            if (!coordinates) {
                continue;
            }

            std::size_t data_index = calculate_index(*coordinates);

            switch (score) {
            case tally_score::flux:
                increment_index(data_index);
                break;
            case tally_score::average_energy:
                update_mean_index(data_index, p.history.energies[i]);
                break;
            default:
                break;
            }
        }
    }
}

uniform_mesh_tally::uniform_mesh_tally(const vec3 &start, const vec3 &end, const coord3 &res,
                                       tally_score sc)
    : resolution(res), score(sc) {

    bounds.min = start;
    bounds.max = end;

    switch (score) {
    case tally_score::flux:
        stride = 1;
        break;
    case tally_score::average_energy:
        stride = 2;
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
    default:
        break;
    }
}

void uniform_mesh_tally::add_particle(const particle &p) {

    switch (score) {
    case tally_score::interaction_counts:
        add_particle_interactionwise(p);
        break;
    case tally_score::flux:
    case tally_score::average_energy:
        add_particle_segmentwise(p);
        break;
    default:
        break;
    }
}

void uniform_mesh_tally::finalize_data() {
    // calculate the average
    if (score == tally_score::average_energy) {
        for (std::size_t i = 0; i < data.size(); i += 2) {
            double sum = std::get<double>(data[i]);
            double count = std::get<double>(data[i]);
            data[i] = sum / count;
        }
    }
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

    for (int z = 0; z < resolution.z(); ++z) {
        for (int y = 0; y < resolution.y(); ++z) {
            for (int x = 0; x < resolution.x(); ++x) {

                output_file << x << "," << y << "," << z;

                std::size_t base = calculate_index({x, y, z});

                for (std::size_t i = 0; i < stride; ++i) {
                    std::visit(print_variant, data[base + i]);
                }

                output_file << "\n";
            }
        }
    }
}

} // namespace projector