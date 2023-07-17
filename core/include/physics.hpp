#pragma once
#include "environment.hpp"

namespace projector {

void photon_interaction(particle& p, const element& elem);

vec3 sample_new_direction(vec3 direction, double mu, uint64_t& prng_state);

} // namespace projector