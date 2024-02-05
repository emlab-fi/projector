#include "runtime.hpp"

#include "constants.hpp"
#include "random_numbers.hpp"

#include <filesystem>
#include <iostream>
#include <stdio.h>
#include<omp.h>

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

    double bb_distance = projector::constants::infinity;
    double surface_distance = projector::constants::infinity;

    // this currently goes through all objects and if we find new nearest BB, we check the surface
    // distance and take it if it is nearer than the previous surface distance
    // it possibly generates false data, need to check/test later
    for (auto &obj : env.objects) {
        double temp_dist = obj.geom.bb.distance_along_line(p.position(), p.direction);


        if (temp_dist < bb_distance) {
            double s_dist = obj.geom.nearest_surface_distance(p.position(), p.direction);
            surface_distance = std::min(surface_distance, s_dist);
            bb_distance = temp_dist;
        }
    }

    if (surface_distance == projector::constants::infinity) {
        return env.bounds.distance_along_line(p.position(), p.direction);
    }

    return surface_distance;
}

} // namespace

namespace projector {


void initialize_runtime(environment &env, int max_threads) {
    thread_count = max_threads;

    omp_set_num_threads(thread_count);

    seed_master_prng(env.seed);

    // sample objects and create particles
    for (object &obj : env.objects) {

        uint64_t prng_local = generate_prng_seed();

        for (std::size_t i = 0; i < obj.photons_activity; ++i) {

            double mu = obj.photons_spread + prng_double(prng_local) * (1.0 - obj.photons_spread);
            double phi = prng_double(prng_local) * 2.0 * constants::pi;
            vec3 direction = rotate_direction(obj.photons_dir, mu, phi);


            particle p = {.particle_type = particle::type::photon,
                          .direction = direction,
                          .prng_state = generate_prng_seed()};
            p.history.elements.push_back(0);
            p.history.energies.push_back(obj.photons_energy);
            p.history.interactions.push_back(cross_section::no_interaction);
            p.history.points.push_back(obj.geom.sample_point(prng_local));

            env.particles.push_back(p);
        }
    }

    for (auto& tally : env.tallies) {
        tally->init_tally();
    }

}

void calculate_particle_histories(environment &env) {

    std::size_t counter = 0;

    // simulate each particle separately
    #pragma omp parallel for
    for (particle &p : env.particles) {

        object *current_obj = get_current_obj(env.objects, p.position());

        for (std::size_t i = 0; i < env.stack_size; ++i) {
            // check for energy cutoff
            if (p.energy() <= env.energy_cutoff) {
                break;
            }

            // check for bounds
            if (!env.bounds.point_inside(p.position())) {
                break;
            }

            // move to nearest surface if we are not in any object
            if (current_obj == nullptr) {
                double dist = nearest_global_distance(env, p);
                p.advance(dist + 5 * constants::epsilon);
                current_obj = get_current_obj(env.objects, p.position());
                continue;
            }

            sampled_xs macro_xs = env.cross_section_data.material_macro_xs(
                env.materials[current_obj->material_id], p.energy());

            double surface_distance =
                current_obj->geom.nearest_surface_distance(p.position(), p.direction);

            double interaction_dist =
                -std::log(prng_double(p.prng_state)) / (macro_xs.total * 10.0e-24);

            // we also need to check whether we dont go out of the bounds
            double env_distance = env.bounds.distance_along_line(p.position(), p.direction);

            if (env_distance < surface_distance && env_distance < interaction_dist) {
                p.advance(env_distance + 5 * constants::epsilon);
            }

            else if (interaction_dist < surface_distance) {
                p.advance(interaction_dist);

                const element &elem = env.cross_section_data.sample_element(
                    env.materials[current_obj->material_id], p.energy(), p.prng_state);

                p.photon_interaction(elem);

            } else {
                // move tiny bit behind the surface, to not get stuck on it
                p.advance(surface_distance + 5 * constants::epsilon);
                current_obj = get_current_obj(env.objects, p.position());
            }
        }

        counter++;
    }
}

void process_tallies(environment &env) {

    for (auto& tally : env.tallies) {
        #pragma omp parallel for
        for (auto& particle : env.particles) {
            tally->add_particle(particle);
        }

        tally->finalize_data();
    }
}

void save_data(environment &env) {

    std::filesystem::create_directories(env.output_path / "tallies");

    for (auto& tally : env.tallies) {
        tally->save_tally(env.output_path / "tallies");
    }

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