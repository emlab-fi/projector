#include "constants.hpp"
#include "tally.hpp"
#include "environment.hpp"

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
#pragma omp atomic
    (*data_storage)[index] += 1;
}

void uniform_mesh_tally::add_index(std::size_t index, double value) {
#pragma omp atomic
    (*data_storage)[index] += value;
}

void uniform_mesh_tally::add_particle_interactionwise(const particle &p, const filter &f) {

    if (!f.check_particle(p)) {
        return;
    }

    auto coord = determine_cell(p.history.points.back());

    if (!coord) {
        return;
    }

    std::size_t data_index = calculate_index(*coord);

    if (score == tally_score::interaction_counts) {
        if (p.history.interactions.back() != cross_section::no_interaction) {

            increment_index(data_index);

            data_index += static_cast<std::size_t>(p.history.interactions.back());

            increment_index(data_index);
        }
    }

    else if (score == tally_score::deposited_energy) {
        double energy_diff = p.history.energies[p.history.energies.size() - 2] - p.energy();
        add_index(data_index, energy_diff);
    }
}

void uniform_mesh_tally::add_particle_segmentwise(const particle &p, const filter& f) {

    std::size_t prev_index = p.history.interactions.size() - 2;

    if (!f.check_particle_index(p, prev_index)) {
        return;
    }

    const vec3 &start = p.history.points[prev_index];
    const vec3 &end = p.history.points.back();

    if (!bounds.segment_intersect(start, end)) {
        return;
    }

    std::vector<double> intersects = calculate_intersections(start, end);

    for (double &t : intersects) {
        // get a point a tiny bit behind the intersection
        vec3 point = start + (t + 50 * constants::epsilon) * (end - start);

        // if we have object filter, check whether we need to count this point of a segment
        if (f.object_id) {
            object* obj = p.history.objects[prev_index];
            if (*f.object_id != obj->id && obj->geom.point_inside(point)) {
                return;
            }
        }

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
            add_index(data_index, p.history.energies[prev_index]);
            increment_index(data_index + 1);
            break;
        default:
            break;
        }
    }
}

uniform_mesh_tally::uniform_mesh_tally(const vec3 &start, const vec3 &end,
                                       const coord3 &res, tally_score sc)
    : resolution(res), score(sc) {

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

void uniform_mesh_tally::init_tally(std::vector<double> *data) {

    std::size_t total = resolution.x() * resolution.y() * resolution.z() * stride;

    data_storage = data;
    data_storage->resize(total, double(0.0));
}

void uniform_mesh_tally::add_particle(const particle &p, const filter &filters) {

    if (!filters.check_particle(p)) {
        return;
    }

    switch (score) {
    case tally_score::interaction_counts:
    case tally_score::deposited_energy:
        add_particle_interactionwise(p, filters);
        break;
    case tally_score::flux:
    case tally_score::average_energy:
        add_particle_segmentwise(p, filters);
        break;
    default:
        throw std::runtime_error("Unsupported tally score for uniform mesh tally");
        break;
    }
}

void uniform_mesh_tally::finalize_data() {
    // calculate the average
    if (score == tally_score::average_energy) {
        for (std::size_t i = 0; i < data_storage->size(); i += 2) {
            double sum = (*data_storage)[i];
            double count = (*data_storage)[i+1];
            (*data_storage)[i] = sum / count;
        }
    }
}

void uniform_mesh_tally::save_tally(std::fstream &output, std::vector<double> &mean,
                                    std::vector<double> &variance) {

    output << "x,y,z,";

    switch(score) {
    case tally_score::average_energy:
        output << "avg_energy mean, avg_energy var, p_count mean, p_count var\n";
        break;
    case tally_score::deposited_energy:
        output << "dep_energy mean, dep_energy var\n";
        break;
    case tally_score::flux:
        output << "flux mean, flux var\n";
        break;
    case tally_score::interaction_counts:
        output << "total mean, total var, coh mean, coh var, incoh mean, incoh var, photoel mean, photoel var, pair_prod mean, pair_prod var\n";
        break;
    default:
        throw std::runtime_error("unsupported tally score for uniform mesh tally");
        return;
    }

    output << "\n";

    output << std::setprecision(10) << std::scientific;

    for (int z = 0; z < resolution.z(); ++z) {
        for (int y = 0; y < resolution.y(); ++y) {
            for (int x = 0; x < resolution.x(); ++x) {

                output << x << "," << y << "," << z;

                std::size_t base = calculate_index({x, y, z});

                for (std::size_t i = 0; i < stride; ++i) {
                    output << "," << mean[base + i] << "," << variance[base+i];
                }

                output << "\n";
            }
        }
    }
}

} // namespace projector