#pragma once
#include "environment.hpp"

namespace projector {

void initialize_runtime(environment &env, int thread_count);

void init_batch(environment &env);

void finish_batch(environment &env);

void calculate_particle_histories(environment &env);

void main_run(environment &env);

void process_tallies(environment &env);

void save_data(environment &env);

} // namespace projector