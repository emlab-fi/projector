#pragma once
#include <cstdint>

namespace projector {

void seed_master_prng(uint64_t seed);

uint64_t generate_prng_seed();

double prng_double(uint64_t& seed);

} //namespace projector