#include "tally.hpp"

namespace projector {

histogram_tally::histogram_tally(std::size_t bins, double max_energy)
    : bins(bins), max_energy(max_energy) {}

void histogram_tally::init_tally(std::vector<double> *data) {
    data_storage = data;
    data_storage->resize(bins, 0.0);
}

void histogram_tally::add_particle(const particle &p, const filter &filters) {
    // check if particle passes through filters
    if (!filters.check_particle(p)) {
        return;
    }

    // check if we have enough points in history to calculate deposited energy
    if (p.history.energies.size() < 2) {
        return;
    }

    // skip interactions that do not deposit energy
    if (p.history.interactions.back() == cross_section::no_interaction ||
        p.history.interactions.back() == cross_section::coherent) {
        return;
    }

    // calculate the bin it the collision goes in
    double deposited = p.history.energies[p.history.energies.size() - 2] - p.energy();
    std::size_t index = std::floor(deposited / (max_energy / bins));

    // clamp the resulting index
    if (index >= bins) {
        index = bins - 1;
    }

    (*data_storage)[index] += 1;
}

void histogram_tally::finalize_data() {
    // normalize data
    double sum = 0.0;

    for (auto& val : *data_storage) {
        sum += val;
    }

    for (auto& val : *data_storage) {
        val /= sum;
    }
    return;
}

void histogram_tally::save_tally(std::fstream &output, std::vector<double> &mean,
                                 std::vector<double> &variance) {

    output << "bin, ratio mean, ratio var\n";

    double step = max_energy / bins;

    output << std::setprecision(10) << std::scientific;
    for (std::size_t i = 0; i < bins; ++i) {
        output << (i + 1) * step << "," << mean[i] << "," << variance[i] << "\n";
    }
}

} // namespace projector