#include "json_loader.hpp"

#include "utils.hpp"

#include <stdexcept>

using json = nlohmann::json;

namespace projector {

void vec3_from_json(json &j, vec3 &vec) {
    if (!j.is_array()) {
        throw std::runtime_error("3d vector is not JSON array");
    }

    double x, y, z;

    j.at(0).get_to(x);
    j.at(1).get_to(y);
    j.at(2).get_to(z);

    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}

void load_simulation_data(std::filesystem::path path, environment &env) {

    json conf = load_json_file(path);

    if (!conf.is_object()) {
        throw std::runtime_error("The top level is not a JSON object");
    }

    conf.at("name").get_to(env.name);
    conf.at("description").get_to(env.description);
    conf.at("seed").get_to(env.seed);
    conf.at("save_particle_paths").get_to(env.save_particle_paths);

    conf.at("energy_cutoff").get_to(env.energy_cutoff);
    conf.at("stack_size").get_to(env.stack_size);

    vec3 min_bb, max_bb;
    from_json(conf.at("bounding_box").at(0), min_bb);
    from_json(conf.at("bounding_box").at(1), min_bb);

    conf.at("material_file").get_to(env.material_path);
    conf.at("object_file").get_to(env.objects_path);
    conf.at("tally_file").get_to(env.tally_path);
    conf.at("output_path").get_to(env.output_path);
};

void load_material_data(std::filesystem::path path, environment &env) {

};

void load_object_data(std::filesystem::path path, environment &env) {

};

void load_tally_data(std::filesystem::path path, environment &env) {

};

} // namespace projector