#include "environment.hpp"

#include <fstream>


constexpr std::string_view interaction_map[] = {"no_interaction", "coherent", "incoherent",
                                                "photoelectric", "pair_production"};

namespace projector {

void particle::save_particle(const std::filesystem::path path) const {

    std::fstream output_file(path, output_file.trunc | output_file.out);

    if (!output_file.is_open()) {
        throw std::runtime_error("failed to open file: " + path.string());
    }

    output_file << "x,y,z,energy,interaction,element" << '\n';

    std::size_t length = history.points.size();

    output_file << std::setprecision(10) << std::scientific;

    for (std::size_t i = 0; i < length; ++i) {
        output_file << history.points[i][0] << ", " << history.points[i][1] << ", "
                    << history.points[i][2] << ", " << history.energies[i] << ", "
                    << interaction_map[static_cast<std::size_t>(history.interactions[i])] << ", "
                    << history.elements[i] << "\n";
    }

    output_file.close();
}

} // namespace projector