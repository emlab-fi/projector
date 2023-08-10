#include "json_loader.hpp"

#include "utils.hpp"

#include <stdexcept>

namespace {

std::size_t find_mat_id(const std::string_view &str,
                        const std::vector<std::string_view> &ids) {

    for (std::size_t index = 0; index < ids.size(); ++index) {
        if (ids[index] == str) {
            return index;
        }
    }

    throw std::runtime_error(std::string("material ID not found:").append(str));
}

} // namespace

using json = nlohmann::json;

namespace projector {

vec3 vec3_from_json(json &j) {
    if (!j.is_array()) {
        throw std::runtime_error("3d vector is not JSON array");
    }

    if ((j.size() != 3)) {
        throw std::runtime_error("Wrong amount of arguments for vec3");
    }

    double x, y, z;

    j.at(0).get_to(x);
    j.at(1).get_to(y);
    j.at(2).get_to(z);

    return {x, y, z};
}

void parse_geometry(geometry &geom, std::string_view key, nlohmann::json &j) {

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


    vec3 min_bb = vec3_from_json(conf.at("bounding_box").at(0));
    vec3 max_bb = vec3_from_json(conf.at("bounding_box").at(1));

    env.bounding_box = {min_bb, max_bb};

    conf.at("material_file").get_to(env.material_path);
    conf.at("object_file").get_to(env.objects_path);
    conf.at("tally_file").get_to(env.tally_path);
    conf.at("output_path").get_to(env.output_path);
}

void load_material_data(std::filesystem::path path, environment &env) {

    json file = load_json_file(path);

    if (!file.is_object()) {
        throw std::runtime_error("The top level is not a JSON object");
    }

    if (!file.at("materials").is_array()) {
        throw std::runtime_error("No array of materials in JSON!");
    }

    for (auto &material : file.at("materials")) {

        if (!material.is_object()) {
            throw std::runtime_error("Material def is not a JSON object");
        }

        std::string_view id_string;
        material.at("id").get_to(id_string);
        env.material_ids.push_back(id_string);

        material_data temp;

        material.at("density").get_to(temp.density);

        std::vector<std::string_view> element_str;

        material.at("elements").get_to(element_str);

        for (auto &elem : element_str) {
            temp.elements.push_back(symbol_to_atomic_number(elem));
        }

        if (material.contains("atomic_percentage")) {
            material.at("atomic_percentage").get_to(temp.atomic_percentage);
        } else if (material.contains("weight_percentage")) {
            material.at("weight_percentage").get_to(temp.weight_percentage);
        } else {
            throw std::runtime_error("Material definition has no percentages");
        }

        env.cross_section_data.material_calculate_missing_values(temp);

        env.materials.push_back(temp);
    }

    if (env.materials.size() != env.material_ids.size()) {
        throw std::runtime_error(
            "Error while loading material data, length mismatch");
    }
}

void load_object_data(std::filesystem::path path, environment &env) {
    json file = load_json_file(path);

    if (!file.is_object()) {
        throw std::runtime_error("The top level is not a JSON object");
    }

    for (auto &obj_json : file.at("objects")) {

        if (!obj_json.is_object()) {
            throw std::runtime_error("object definition is not a JSON object");
        }

        object new_obj;

        obj_json.at("id").get_to(new_obj.id);

        new_obj.photons_activity = 0;
        new_obj.photons_energy = 0.0;

        if (obj_json.contains("source")) {

            obj_json.at("source")
                .at("photon_count")
                .get_to(new_obj.photons_activity);

            obj_json.at("source")
                .at("photon_energy")
                .get_to(new_obj.photons_energy);
        }

        std::string_view material_string;
        obj_json.at("material_id").get_to(material_string);

        new_obj.material_id = find_mat_id(material_string, env.material_ids);

        std::string_view geom_key;
        obj_json.at("geometry").get_to(geom_key);

        parse_geometry(new_obj.geom, geom_key, file.at("geometries"));

        env.objects.push_back(std::move(new_obj));
    }
}

void load_tally_data(std::filesystem::path path, environment &env) {}

} // namespace projector