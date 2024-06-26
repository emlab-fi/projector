#include "constants.hpp"
#include "tally.hpp"

#include <fstream>
#include <type_traits>

namespace {

double distance_to_aligned_plane(double x, double x_0, double u) {

    if (u == 0.0) {
        return projector::constants::infinity;
    }
    double distance = (x_0 - x) / u;

    if (distance < 0.0 || distance > 1.0) {
        return projector::constants::infinity;
    }
    return distance;
}

} // namespace

namespace projector {

std::optional<coord3> uniform_mesh_tally::determine_cell(const vec3 &point) const {
    if (!bounds.point_inside(point)) {
        return {};
    }

    coord3 output = {0, 0, 0};

    for (std::size_t i = 0; i < 3; ++i) {
        double shifted = point[i] - bounds.min[i];
        double step_size = std::abs(bounds.max[i] - bounds.min[i]) / resolution[i];
        double out = std::floor(shifted / step_size);

        output[i] = out;
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

    vec3 dir = end - start;
    std::vector<double> out;

    for (std::size_t index = 0; index < 3; ++index) {
        double step = (bounds.max[index] - bounds.min[index]) / resolution[index];

        for (int i = 0; i < resolution[index]; ++i) {
            double temp_dist =
                distance_to_aligned_plane(start[index], bounds.min[index] + i * step, dir[index]);
            if (temp_dist == projector::constants::infinity) {
                continue;
            }
            out.push_back(temp_dist);
        }
    }
    return out;
}

void uniform_mesh_tally::increment_index(std::size_t index) {

    auto increment_visit = [](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_arithmetic_v<T>) {
#pragma omp atomic
            arg += 1;
        }
    };

    { std::visit(increment_visit, data[index]); }
}

void uniform_mesh_tally::add_index(std::size_t index, double value) {

    auto add_visit = [&value](auto &&arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_arithmetic_v<T>) {
#pragma omp atomic
            arg += value;
        }
    };

    { std::visit(add_visit, data[index]); }
}

void uniform_mesh_tally::add_particle_interactionwise(const particle &p) {
    std::size_t amount = p.history.points.size();

    // start at index 1, as index 0 is always initial particle state and that does not do anything
    for (std::size_t i = 1; i < amount; ++i) {

        auto coord = determine_cell(p.history.points[i]);

        if (!coord) {
            continue;
        }

        std::size_t data_index = calculate_index(*coord);

        if (score == tally_score::interaction_counts) {
            if (p.history.interactions[i] != cross_section::no_interaction) {

                increment_index(data_index);

                data_index += static_cast<std::size_t>(p.history.interactions[i]);

                increment_index(data_index);
            }
        }

        else if (score == tally_score::deposited_energy) {
            double energy_diff = p.history.energies[i - 1] - p.history.energies[i];
            add_index(data_index, energy_diff);
        }
    }
}

void uniform_mesh_tally::add_particle_segmentwise(const particle &p) {

    std::size_t amount = p.history.points.size() - 1;

    // change to DDA algorithm!
    for (std::size_t i = 0; i < amount; ++i) {

        const vec3 &start = p.history.points[i];
        const vec3 &end = p.history.points[i + 1];

        if (!bounds.segment_intersect(start, end)) {
            continue;
        }

        std::vector<double> intersects = calculate_intersections(start, end);

        for (double &t : intersects) {
            // get a point a tiny bit behind the intersection
            vec3 point = start + (t + 50 * constants::epsilon) * (end - start);

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
                add_index(data_index, p.history.energies[i]);
                increment_index(data_index + 1);
                break;
            default:
                break;
            }
        }
    }
}

uniform_mesh_tally::uniform_mesh_tally(std::string user_id, const vec3 &start, const vec3 &end,
                                       const coord3 &res, tally_score sc)
    : id(user_id), resolution(res), score(sc) {

    bounds.min = start;
    bounds.max = end;

    switch (score) {
    case tally_score::flux:
    case tally_score::deposited_energy:
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
    case tally_score::deposited_energy:
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
    case tally_score::deposited_energy:
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
            double count = std::get<double>(data[i + 1]);
            data[i] = sum / count;
        }
    }
}

void uniform_mesh_tally::save_tally(const std::filesystem::path path) const {

    std::string filename = id + ".csv";

    std::fstream output_file(path / filename, output_file.trunc | output_file.out);

    if (!output_file.is_open()) {
        throw std::runtime_error("failed to open file: " + path.string());
    }

    output_file << "x,y,z";
    for (std::size_t i = 0; i < stride; ++i) {
        output_file << ",data" << i;
    }
    output_file << "\n";

    output_file << std::setprecision(10) << std::scientific;

    auto print_variant = [&output_file]<typename T>(T &&arg) { output_file << "," << arg; };

    for (int z = 0; z < resolution.z(); ++z) {
        for (int y = 0; y < resolution.y(); ++y) {
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