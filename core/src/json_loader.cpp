#include "json_loader.hpp"

#include "utils.hpp"

#include <stdexcept>

namespace {

std::size_t find_mat_id(const std::string &str,
                        const std::vector<std::string> &ids) {

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


std::unique_ptr<surface> parse_surface(json &j) {

    std::string_view surf_type = j.at("type").get<std::string_view>();

    vec3 center = vec3_from_json(j.at("parameters")[0]);

    if (surf_type == "plane") {
        vec3 normal = vec3_from_json(j.at("parameters")[1]);
        return std::make_unique<plane>(center, normal);
    }

    double a = j.at("parameters")[1].get<double>();
    double b = j.at("parameters")[2].get<double>();
    double c = 0.0;
    if (j.at("parameters").size() >= 4) {
        c = j.at("parameters")[3].get<double>();
    }

    if (surf_type == "ellipsoid") {
        return std::make_unique<ellipsoid>(center, a, b, c);
    }
    if (surf_type == "x_cylinder") {
        return std::make_unique<x_cylinder>(center, a, b);
    }
    if (surf_type == "y_cylinder") {
        return std::make_unique<y_cylinder>(center, a, b);
    }
    if (surf_type == "z_cylinder") {
        return std::make_unique<z_cylinder>(center, a, b);
    }
    if (surf_type == "x_cone") {
        return std::make_unique<x_cone>(center, a, b, c);
    }
    if (surf_type == "y_cone") {
        return std::make_unique<y_cone>(center, a, b, c);
    }
    if (surf_type == "z_cone") {
        return std::make_unique<z_cone>(center, a, b, c);
    }

    throw std::runtime_error("invalid surface type");
}

void parse_geometry(geometry &geom, std::string_view key, nlohmann::json &j) {

    auto& geom_json = j.at(key);

    if (!geom_json.is_object()) {
        throw std::runtime_error("geometry is not JSON object");
    }

    std::size_t surface_count = geom_json.at("operators").size();

    for (std::size_t i = 0; i < surface_count; ++i) {

        auto& current_surface = geom_json.at("surfaces")[i];

        if (current_surface.is_string()) {

            geometry g;
            std::string_view new_key;

            current_surface.get_to(new_key);

            parse_geometry(g, new_key, j);

            geom.add_surface(std::move(g), geom_json.at("operators")[i]);

        } else if (current_surface.is_object()) {

            if (!current_surface.at("parameters").is_array()) {
                throw std::runtime_error("surface parameters are not array");
            }

            std::unique_ptr<surface> surf = parse_surface(current_surface);

            geom.add_surface(std::move(surf), geom_json.at("operators")[i]);

        } else {
            throw std::runtime_error("invalid type for surface definition");
        }

    }

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

        std::string id_string;
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

        std::string material_string;
        obj_json.at("material_id").get_to(material_string);

        new_obj.material_id = find_mat_id(material_string, env.material_ids);

        std::string geom_key;
        obj_json.at("geometry").get_to(geom_key);

        parse_geometry(new_obj.geom, geom_key, file.at("geometries"));

        vec3 min_bb, max_bb;

        if (obj_json.contains("bounding_box")) {
            min_bb = vec3_from_json(obj_json.at("bounding_box").at(0));
            max_bb = vec3_from_json(obj_json.at("bounding_box").at(1));
        } else {
            min_bb = env.bounding_box.first;
            max_bb = env.bounding_box.second;
        }

        new_obj.geom.update_bounding_box(min_bb, max_bb);

        env.objects.push_back(std::move(new_obj));
    }
}

void load_tally_data(std::filesystem::path path, environment &env) {}

} // namespace projector