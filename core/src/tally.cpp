#include "tally.hpp"

#include <fstream>

namespace projector {

tally_manager::tally_manager(std::string usr_id, std::unique_ptr<tally> tally_type,
                             std::vector<std::unique_ptr<filter>> filters)
    : id(usr_id), tally_type(std::move(tally_type)), filters(filters) {}

void tally_manager::init_batch() {
    data.emplace_back();
    tally_type->init_tally(&data.back());
}

void tally_manager::finalize_batch() { tally_type->finalize_data(); }

void tally_manager::add_particle(const particle &p) { tally_type->add_particle(p, filters); }

void tally_manager::finalize_data() {
    std::size_t batches = data.size();
    std::size_t data_length = data[0].size();

    final_mean = std::vector<double>(data_length);
    final_variance = std::vector<double>(data_length);


    for (std::size_t i = 0; i < data_length; ++i) {
        double running_sum = 0;

        for (std::size_t b = 0; b < batches; ++i) {
            running_sum += data[b][i];
        }

        final_mean[i] = running_sum / batches;
        running_sum = 0;

        for (std::size_t b = 0; b < batches; ++i) {
            double var = data[b][i] - final_mean[i];
            running_sum += var * var;
        }

        final_variance[i] = running_sum / batches;
    }
}

void tally_manager::save_tally(const std::filesystem::path path) {
    std::string filename = id + ".csv";

    std::fstream output_file(path / filename, output_file.trunc | output_file.out);

    if (!output_file.is_open()) {
        throw std::runtime_error("failed to open file: " + path.string());
    }

    tally_type->save_tally(output_file, final_mean, final_variance);
}

} // namespace projector