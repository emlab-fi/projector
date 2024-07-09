#pragma once
#include "geometry.hpp"
#include "material.hpp"

#include <vector>

namespace projector {

/// @brief struct storing the history of a single particle
///
/// Particle histories are stores event-wise, as we can fully reconstruct their track from this
/// particle creation is at index 0, newest even at highest index
struct particle_history {
    std::vector<vec3> points;
    std::vector<double> times;
    std::vector<double> energies;
    std::vector<cross_section> interactions;
    std::vector<std::size_t> elements;
};

/// @brief Single particle representation
///
/// Particle representation is minimal - keep only the history, type and it's RNG state
struct particle {

    enum class type { photon, positron, electron };

    type particle_type;

    vec3 direction;

    uint64_t prng_state;

    particle_history history;

    /// get current particle time
    double &time();

    /// get current particle energy
    double &energy();

    /// get current particle position
    vec3 &position();

    /// Save particle history to file
    /// @param path path to the output file
    void save_particle(const std::filesystem::path path) const;

    /// Calculate photon interaction with an element
    /// @param elem the element to interact with
    void photon_interaction(const element &elem);

    /// Advance particle in space and time
    /// @param distance the distance to advance by
    void advance(double distance);
};


} // namespace projector