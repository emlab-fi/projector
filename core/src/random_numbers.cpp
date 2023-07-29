#include "random_numbers.hpp"

#include <cmath>

uint64_t master_seed = 1;

constexpr uint64_t prng_mult = 6364136223846793005ull;
constexpr uint64_t prng_add = 1442695040888963407ull;
constexpr uint64_t prng_permute_mult = 12605985483714917081ull;

namespace {

// adapted from PCG implementation, see https://www.pcg-random.org
// rxs_m_xs_64_64 output function variant
uint64_t advance_prng(uint64_t &state) {
    // advance the state
    state = state * prng_mult + prng_add;

    // generate output
    uint64_t output =
        (state >> ((state >> 59u) + 5u) ^ state) * prng_permute_mult;
    output = (output >> 43u) ^ output;

    return output;
}

} // namespace

namespace projector {

void seed_master_prng(uint64_t seed) { master_seed = seed; }

uint64_t generate_prng_seed() { return advance_prng(master_seed); }

double prng_double(uint64_t &state) {

    uint64_t output = advance_prng(state);

    return std::ldexp(output, -64);
};

double prng_double() {
    uint64_t output = advance_prng(master_seed);

    return std::ldexp(output, -64);
}

} // namespace projector