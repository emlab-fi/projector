#include "tally.hpp"

namespace projector {

volume_tally::volume_tally(tally_score sc) : score(sc) {}

void volume_tally::init_tally(std::vector<double> *data) {
    data_storage = data;
    switch (score) {
    case tally_score::flux:
    case tally_score::deposited_energy:
        data_storage->resize(1, 0.0);
        break;
    case tally_score::average_energy:
        data_storage->resize(2, 0.0);
        break;
    case tally_score::interaction_counts:
        data_storage->resize(5, 0.0);
        break;
    default:
        throw std::runtime_error("Unsupported tally score for volume tally");
        return;
    }
}

void volume_tally::add_particle(const particle &p, const filter &filters) {
    // check whether particle passes through filters
    if (!filters.check_particle(p)) {
        return;
    }
    // if we count flux, we only care when the particle first arrives
    // creates false positives on non convex geometries
    if (score == tally_score::flux) {
        auto prev_geom = p.history.objects[p.history.objects.size() - 2];
        if (prev_geom == p.current_object()) {
            return;
        }
    }

    switch(score) {
    case tally_score::average_energy:
        (*data_storage)[0] += p.energy();
        (*data_storage)[1] += 1.0;
        break;
    case tally_score::deposited_energy:
        if (p.history.interactions.back() == cross_section::no_interaction) {
            break;
        }
        if (p.history.energies.size() < 2) {
            break;
        }
        double deposited = p.history.energies[p.history.energies.size() - 2] - p.energy();
        (*data_storage)[0] += deposited;
        break;
    case tally_score::flux:
        (*data_storage)[0] += 1.0;
        break;
    case tally_score::interaction_counts:
        if (p.history.interactions.back() == cross_section::no_interaction) {
            break;
        }
        (*data_storage)[0] += 1.0;
        std::size_t offset = static_cast<std::size_t>(p.history.interactions.back());
        (*data_storage)[offset] += 1.0;
        break;
    default:
        throw std::runtime_error("unsupported tally score for volume tally!");
        return;
    }
}

void volume_tally::finalize_data() {
    if (score == tally_score::average_energy) {
        (*data_storage)[0] /= (*data_storage)[1];
    }
    return;
}

void volume_tally::save_tally(std::fstream &output, std::vector<double> &mean,
                              std::vector<double> &variance) {

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
        throw std::runtime_error("unsupported tally score for volume tally!");
        return;
    }

    output << std::setprecision(10) << std::scientific;

    for (std::size_t i = 0; i < mean.size(); ++i) {
        output << mean[i] << "," << variance[i] << "\n";
    }
}

} // namespace projector