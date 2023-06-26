#include <cmath>
#include "random_numbers.hpp"

uint64_t master_seed = 1;

uint64_t prng_mult = 12605985483714917081ull;
uint64_t prng_add = 1;
uint64_t prng_permute_mult = 12605985483714917081ull;

namespace projector {

void seed_master_prng(uint64_t seed) {
    master_seed = seed;
}


// adapted from PCG implementation, see https://www.pcg-random.org
// rxs_m_xs_64_64 output function variant
double prng_double(uint64_t& state) {

    // advance the state
    state = state * prng_mult + prng_add;

    // generate output
    uint64_t output = (state >> ((state >> 59u) + 5u) ^ state) * prng_permute_mult;
    output = (output >> 43u) ^ output;

    return std::ldexp(output, -64);
};

}