#include <stdexcept>
#include "json_converters.hpp"
#include "utils.hpp"

using json = nlohmann::json;

namespace projector {

void from_json(json& j, environment& env) {
    if (!j.is_object()) {
        throw std::runtime_error("environment is not JSON object");
    }

    if (!j.at("objects").is_array()) {
        throw std::runtime_error("objects is not JSON array");
    }

    if (!j.at("tallies").is_array()) {
        throw std::runtime_error("tallies is not JSON array");
    }

    j.at("name").get_to(env.name);
    j.at("description").get_to(env.description);
    j.at("output").get_to(env.output_path);
    j.at("save_particle_paths").get_to(env.save_particle_paths);
    j.at("seed").get_to(env.seed);
    j.at("energy_cutoff").get_to(env.energy_cutoff);
    j.at("stack_size").get_to(env.stack_size);

    for (std::size_t i = 0; i < j.at("objects").size(); ++i) {
        object obj;
        from_json(j.at("objects").at(i), obj);
        env.objects.emplace_back(std::move(obj));
    }

    for (std::size_t i = 0; i < j.at("tallies").size(); ++i) {
        tally ta;
        from_json(j.at("tallies").at(i), ta);
        env.tallies.emplace_back(ta);
    }
}


void from_json(json& j, tally& ta) {
    if (!j.is_object()) {
        throw std::runtime_error("tally is not JSON object");
    }

    j.at("type").get_to(ta.tally);

    from_json(j.at("cell_size"), ta.cell_size);
    from_json(j.at("start"), ta.start);

    if (!j.at("cell_count").is_array()) {
        throw std::runtime_error("cell count is not JSON array");
    }

    j.at("cell_count").at(0).get_to(ta.x_count);
    j.at("cell_count").at(1).get_to(ta.y_count);
    j.at("cell_count").at(2).get_to(ta.z_count);
}


void from_json(json& j, object& obj) {
    if (!j.is_object()) {
        throw std::runtime_error("geom object is not JSON object");
    }

    j.at("id").get_to(obj.id);
    from_json(j.at("material"), obj.material);
    j.at("properties").at("material").get_to(obj.material_string);
    j.at("properties").at("photons_activity").get_to(obj.photons_activity);
    j.at("properties").at("photons_energy").get_to(obj.photons_energy);
    from_json(j.at("geometry"), (obj.geom));
}


void from_json(nlohmann::json&j, material_data& mat) {
    if (!j.is_object()) {
        throw std::runtime_error("material object is not JSON object");
    }

    j.at("density").get_to(mat.density);

    std::vector<std::string_view> elems;

    j.at("elements").get_to(elems);
    if (j.contains("atomic_percentage")) {
        j.at("atomic_percentage").get_to(mat.atomic_percentage);
    }
    else if (j.contains("weight_percentage")) {
        j.at("weight_percentage").get_to(mat.weight_percentage);
    }
    else {
        throw std::runtime_error("No percentages for material definition");
    }

    for (auto& elem : elems) {

        std::size_t elem_number = symbol_to_atomic_number(elem);

        if (elem_number == 0) {
            std::string error = "Not a valid symbol: ";
            throw std::runtime_error(error.append(elem));
        }

        mat.elements.push_back(elem_number);
    }

}


void from_json(json& j, geometry& geom) {
    if (!j.is_object()) {
        throw std::runtime_error("geometry is not JSON object");
    }

    if (j.at("type") == "primitive") {
        geom_primitive prim;
        from_json(j.at("properties"), prim);
        geom.definition = prim;
    }

    else if (j.at("type") == "operation") {
        operation op;
        from_json(j.at("properties"), op);
        geom.definition = std::move(op);
    }

    else {
        throw std::runtime_error("Unknown geometry type in JSON");
    }
}


void from_json(json& j, geom_primitive& shape) {
    if (!j.is_object()) {
        throw std::runtime_error("geom primitive properties is not JSON object");
    }

    if (!j.at("parameters").is_array()) {
        throw std::runtime_error("parameters is not JSON array");
    }

    j.at("type").get_to(shape.type);
    from_json(j.at("parameters").at(0), shape.param1);
    from_json(j.at("parameters").at(1), shape.param2);
}


void from_json(json& j, operation& op) {
    if (!j.is_object()) {
        throw std::runtime_error("operation properties is not JSON object");
    }

    j.at("type").get_to(op.op);

    std::unique_ptr<geometry> left = std::make_unique<geometry>();
    std::unique_ptr<geometry> right = std::make_unique<geometry>();

    from_json(j.at("left"), *left);
    from_json(j.at("left"), *right);

    op.left = std::move(left);
    op.right = std::move(right);
}


void from_json(json& j, vec3& vec) {
    if (!j.is_array()) {
        throw std::runtime_error("3d vector is not JSON array");
    }

    double x,y,z;

    j.at(0).get_to(x);
    j.at(1).get_to(y);
    j.at(2).get_to(z);

    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
}

}