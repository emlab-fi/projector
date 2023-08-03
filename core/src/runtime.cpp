#include "runtime.hpp"

#include "random_numbers.hpp"

#include <filesystem>
#include <iostream>
#include <stdio.h>

int thread_count = 1;

namespace {

projector::object *get_current_obj(std::vector<projector::object> &objs,
                                   projector::vec3 current_pos) {
    projector::object *found = nullptr;

    // go backwards in search, the first object is a global bounding box
    for (int i = objs.size() - 1; i >= 0; --i) {
        if (objs[i].geom.point_inside(current_pos)) {
            found = &objs[i];
            break;
        }
    }

    return found;
}

} // namespace

namespace projector {


void initialize_runtime(environment &env, int max_threads) {
    thread_count = max_threads;

    seed_master_prng(env.seed);

    // sample objects and create particles
    for (object &obj : env.objects) {

        uint64_t prng_local = generate_prng_seed();

        for (std::size_t i = 0; i < obj.photons_activity; ++i) {

            particle p = {
                .particle_type = particle::type::photon,
                .current_direction = random_unit_vector(prng_local),
                .prng_state = generate_prng_seed(),
            };
            p.history.elements.push_back(0);
            p.history.energies.push_back(obj.photons_energy);
            p.history.interactions.push_back(cross_section::no_interaction);
            p.history.points.push_back(obj.geom.sample_point(prng_local));

            env.particles.push_back(p);
        }
    }

    // allocate memory for particle histories (max stack size)
    for (particle &p : env.particles) {
        p.history.elements.reserve(env.stack_size);
        p.history.energies.reserve(env.stack_size);
        p.history.interactions.reserve(env.stack_size);
        p.history.points.reserve(env.stack_size);
    }
}

void calculate_particle_histories(environment &env) {

    // simulate each particle separately
    for (particle &p : env.particles) {

        object *current_obj =
            get_current_obj(env.objects, p.current_position());

        for (std::size_t i = 0; i < env.stack_size; ++i) {
            // check for energy cutoff
            if (p.current_energy() <= env.energy_cutoff) {
                break;
            }

            // check if we ran out of bounds
            if (current_obj == nullptr) {
                break;
            }

            sampled_xs macro_xs = env.cross_section_data.material_macro_xs(
                current_obj->material, p.current_energy());

            // multiply by 10e-24 to convert barns to cm2
            p.advance(macro_xs.total * 10.0e-24);

            current_obj = get_current_obj(env.objects, p.current_position());

            // check if we ran out of bounds
            if (current_obj == nullptr) {
                break;
            }

            const element &elem = env.cross_section_data.sample_element(
                current_obj->material, p.current_energy(), p.prng_state);

            p.photon_interaction(elem);
        }
    }
}

void process_tallies(environment &env) {}

void save_data(environment &env) {

    // check if we can skip saving particle data
    if (!env.save_particle_paths) {
        return;
    }

    for (std::size_t i = 0; i < env.particles.size(); ++i) {
        char file_path[256] = {0};

        snprintf(file_path, 256, "photon_%09zu.csv", i);

        std::filesystem::create_directories(env.output_path / "tracks");

        env.particles[i].save_particle(env.output_path / "tracks" / file_path);
    }
}

} // namespace projector