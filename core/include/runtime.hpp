#pragma once
#include "environment.hpp"

namespace projector {

void initialize_runtime(environment& env);

void calculate_particle_histories(environment& env);

void process_tallies(environment& env);

void save_data(environment& env);

} // namespace projector