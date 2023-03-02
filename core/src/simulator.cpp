#include <omp.h>
#include <random>
#include <stdexcept>
#include <iostream>
#include <numbers>
#include <indicators/progress_bar.hpp>
#include "simulator.hpp"

namespace {
    uint32_t xorshift32(uint32_t seed) {
        uint32_t x = seed;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return x;
    }

    std::unique_ptr<double[]> apply_noise(std::unique_ptr<double[]> data, std::unique_ptr<double[]> noise_data, size_t size, std::pair<double, double> coefficients) {
        double max = 0.0;
        for (size_t i = 0; i < size; ++i) {
            max = std::max(max, data[i]);
        }


        for (size_t i = 0; i < size; ++i) {
            double log_coeff = std::pow(std::log(data[i]/max + std::numbers::e), coefficients.second);
            double coeff = (coefficients.first / log_coeff) * noise_data[i];
            double noise = data[i] * coeff;
            data[i] = data[i] + std::abs(noise);
        }

        return data;
    }
}


bool evalute_ray(const geom::ray& r, const geom::z_cylinder& target, const std::vector<blocker>& blockers, double rand_val) {
    double transmitted = 1.0;

    for (const auto& block : blockers) {
        bool hit = false;

        if (geom::ray_intersects_z_area(r, block.shape)) {
            hit = true;
            for (const auto& hole : block.holes) {
                if (geom::ray_intersects_z_area(r, hole)) {
                    hit = false;
                    break;
                }
            }
        }
        if (hit) {
            transmitted *= 1 - block.attenuation;
        }
    }
    if (rand_val <= 1 - transmitted) {
        return false;
    }

    return geom::ray_intersects_z_cylinder(r, target).has_value();
}

std::unique_ptr<double[]> run_simulation(simulation_context& env, int thread_count) {

    size_t x_steps = (env.stop[0] - env.start[0]) / env.resolution;
    size_t y_steps = (env.stop[1] - env.start[1]) / env.resolution;
    size_t total_steps = x_steps * y_steps;

    auto sphere_points = geom::sphere_fibonnaci(env.source_activity);

    auto output = std::make_unique<double[]>(total_steps);

    //create RNG for probabilities and noise
    std::random_device rand_dev;
    std::default_random_engine rand_eng(rand_dev());
    std::uniform_real_distribution<double> uni_pos(0.0, 1.0);
    std::normal_distribution<double> norm_dist(0.0, 1.0);

    //generate all needed values first
    auto probabilities = std::make_unique<double[]>(env.source_activity);
    auto noise = std::make_unique<double[]>(total_steps);

    for (size_t i = 0; i < env.source_activity; ++i) {
        probabilities[i] = uni_pos(rand_eng);
    }

    for (size_t i = 0; i < total_steps; ++i) {
        noise[i] = std::clamp(norm_dist(rand_eng), -3.0, 3.0) / 3.0;
    }

    //create indicator bar
    indicators::ProgressBar pbar{
        indicators::option::BarWidth{50},
        indicators::option::ShowElapsedTime{true},
        indicators::option::ShowRemainingTime{true},
        indicators::option::PrefixText{"Simulating..."},
        indicators::option::ForegroundColor{indicators::Color::white},
        indicators::option::MaxProgress{total_steps}
    };

    omp_set_num_threads(thread_count);

    #pragma omp parallel for collapse(2)
    for (size_t y = 0; y < y_steps; ++y) {
        for (size_t x = 0; x < x_steps; ++x) {

            size_t index = y * x_steps + x;
            double counter = 0;

            size_t prob_index = 0;
            uint32_t shift = xorshift32(omp_get_thread_num() + index);

            for (auto const& dir : sphere_points) {

                geom::point ray_start;
                switch (env.type) {
                    case scan_type::sensor_shift:
                        ray_start = env.source;
                        break;
                    case scan_type::sensor_source_shift:
                        ray_start = {env.start[0] + x * env.resolution, env.start[1] + y * env.resolution, env.source[2]};
                        break;
                    default:
                        ray_start = Eigen::Vector3d(0);
                }

                const geom::ray r = {
                    ray_start,
                    dir
                };

                const geom::z_cylinder c = {
                    {env.start[0] + x * env.resolution, env.start[1] + y * env.resolution, env.start[2]},
                    env.scintillator_size.first,
                    env.scintillator_size.second
                };
                size_t current_index = (prob_index + shift) % env.source_activity;
                prob_index++;
                if (evalute_ray(r, c, env.blockers, probabilities[current_index])) counter += 1.0;
            }
            output[index] = counter;
            pbar.tick();
        }
    }

    return apply_noise(std::move(output), std::move(noise), total_steps, env.noise_coefficients);
}

void from_json(const nlohmann::json& j, blocker& b) {
    if (!j.at("holes").is_array()) {
        throw std::invalid_argument("JSON blocker holes is not an array.");
    }

    j.at("attenuation").get_to(b.attenuation);
    j.at("shape").get_to(b.shape);
    j.at("holes").get_to(b.holes);
}

void from_json(const nlohmann::json& j, simulation_context& env) {
    if (!j.is_object()) {
        throw std::invalid_argument("JSON is not an object.");
    }
    if (!j.at("blockers").is_array()) {
        throw std::invalid_argument("JSON blockers is not an array.");
    }

    if (j.at("type") == "sensor_shift") env.type = scan_type::sensor_shift;
    else if (j.at("type") == "target_shift") env.type = scan_type::sensor_source_shift;
    else throw std::invalid_argument("JSON scan type is not supported.");

    geom::from_json(j.at("start_pos"), env.start);
    geom::from_json(j.at("end_pos"), env.stop);
    j.at("resolution").get_to(env.resolution);
    j.at("noise_coefficients").at(0).get_to(env.noise_coefficients.first);
    j.at("noise_coefficients").at(1).get_to(env.noise_coefficients.second);
    geom::from_json(j.at("source"), env.source);
    j.at("source_activity").get_to(env.source_activity);
    j.at("scintilator_size").at(0).get_to(env.scintillator_size.first);
    j.at("scintilator_size").at(1).get_to(env.scintillator_size.second);
    j.at("blockers").get_to(env.blockers);
}
