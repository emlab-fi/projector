#pragma once
#include "geometry.hpp"
#include "material.hpp"
#include "environment.hpp"

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
    std::vector<object*> objects;
};

/// @brief Single particle representation
struct particle {

    enum class type { photon, positron, electron };

    type particle_type;

    vec3 direction;

    uint64_t prng_state;

    particle_history history;

    source const * const source;

    /// get current particle time
    double &time();
    const double &time() const;

    /// get current particle energy
    double &energy();
    const double &energy() const;

    /// get current particle position
    vec3 &position();
    const vec3 &position() const;

    /// get pointer to object the particle is in
    object *current_object();

    /// update the current object of particle
    /// @param new_object pointer ot the new object
    void update_object(object * new_object);

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