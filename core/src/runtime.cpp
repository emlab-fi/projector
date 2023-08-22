#include "runtime.hpp"

#include "constants.hpp"
#include "random_numbers.hpp"

#include <filesystem>
#include <iostream>
#include <stdio.h>

int thread_count = 1;

namespace {

projector::object *get_current_obj(std::vector<projector::object> &objs,
                                   projector::vec3 current_pos) {
    projector::object *found = nullptr;

    // go backwards in search
    for (int i = objs.size() - 1; i >= 0; --i) {
        if (objs[i].geom.point_inside(current_pos)) {
            found = &objs[i];
            break;
        }
    }

    return found;
}

double nearest_global_distance(projector::environment &env, projector::particle &p) {

    projector::object *nearest = nullptr;

    double min_distance = projector::constants::infinity;

    for (std::size_t i = 0; i < env.objects.size(); ++i) {
        double temp_dist =
            env.objects[i].geom.bb.distance_along_line(p.current_position(), p.current_direction);

        if (temp_dist < min_distance) {
            min_distance = temp_dist;
            nearest = &env.objects[i];
        }
    }

    if (nearest == nullptr) {
        return env.bounds.distance_along_line(p.current_position(), p.current_direction);
    }

    return nearest->geom.nearest_surface_distance(p.current_position(), p.current_direction);
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
                .prng_state = generate_prng_seed()
            };
            p.history.elements.push_back(0);
            p.history.energies.push_back(obj.photons_energy);
            p.history.interactions.push_back(cross_section::no_interaction);
            p.history.points.push_back(obj.geom.sample_point(prng_local));

            env.particles.push_back(p);
        }
    }
}

void calculate_particle_histories(environment &env) {

    std::size_t counter = 0;

    // simulate each particle separately
    for (particle &p : env.particles) {

        object *current_obj = get_current_obj(env.objects, p.current_position());

        for (std::size_t i = 0; i < env.stack_size; ++i) {
            // check for energy cutoff
            if (p.current_energy() <= env.energy_cutoff) {
                break;
            }

            // check for bounds
            if (!env.bounds.point_inside(p.current_position())) {
                break;
            }

            // move to nearest surface if we are not in any object
            if (current_obj == nullptr) {
                double dist = nearest_global_distance(env, p);
                p.advance(dist + 5 * constants::epsilon);
                continue;
            }

            std::cout << current_obj->id << std::endl;

            sampled_xs macro_xs = env.cross_section_data.material_macro_xs(
                env.materials[current_obj->material_id], p.current_energy());

            double surface_distance = current_obj->geom.nearest_surface_distance(
                p.current_position(), p.current_direction);

            double interaction_dist =
                -std::log(prng_double(p.prng_state)) / (macro_xs.total * 10.0e-24);

            if (interaction_dist < surface_distance) {
                p.advance(interaction_dist);

                const element &elem = env.cross_section_data.sample_element(
                    env.materials[current_obj->material_id], p.current_energy(), p.prng_state);

                p.photon_interaction(elem);

            } else {
                // move tiny bit behind the surface, to not get stuck on it
                p.advance(surface_distance + 5 * constants::epsilon);
                current_obj = get_current_obj(env.objects, p.current_position());
            }
        }

        counter++;
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