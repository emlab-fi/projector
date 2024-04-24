#include "plot.hpp"

#include <fstream>

namespace projector {

void env_slice_plot(const environment &env, const char plane, double center,
                    std::size_t res_x, std::size_t res_y, const std::filesystem::path output) {


    std::fstream output_file(output, output_file.trunc | output_file.out);

    if (!output_file.is_open()) {
        throw std::runtime_error("failed to open file: " + output.string());
    }

    output_file << "x,y,z,material_id,object_id" << "\n";


    // this remaps axes to XY of the output "picture"
    // we check centerpoint of the "pixel", so we also add a constant axis element
    vec3 x_increment;
    vec3 y_increment;
    vec3 constant_axis;
    double x_step, y_step;

    switch (plane) {
    // X plane - YZ axis active, X axis constant
    case 'x':
        x_step = (env.bounds.max[1] - env.bounds.min[1]) / res_x;
        y_step = (env.bounds.max[2] - env.bounds.min[2]) / res_y;
        x_increment = {0.0, x_step, 0.0};
        y_increment = {0.0, 0.0, y_step};
        constant_axis = {center, env.bounds.min[1] + x_step/2.0, env.bounds.min[2] + y_step/2.0};
    break;

    // Y plane - XZ axis active, Y axis constant
    case 'y':
        x_step = (env.bounds.max[0] - env.bounds.min[0]) / res_x;
        y_step = (env.bounds.max[2] - env.bounds.min[2]) / res_y;
        x_increment = {x_step, 0.0, 0.0};
        y_increment = {0.0, 0.0, y_step};
        constant_axis = {env.bounds.min[0] + x_step/2.0, center, env.bounds.min[2] + y_step/2.0};
    break;

    // Z plane - XY axis active, Z axis constant
    case 'z':
        x_step = (env.bounds.max[0] - env.bounds.min[0]) / res_x;
        y_step = (env.bounds.max[1] - env.bounds.min[1]) / res_y;
        x_increment = {x_step, 0.0, 0.0};
        y_increment = {0.0, y_step, 0.0};
        constant_axis = {env.bounds.min[0] + x_step/2.0, env.bounds.min[1] + y_step/2.0, center};
    break;
    default:
        throw std::runtime_error("invalid slice plane!");
    }

    for (std::size_t y = 0; y < res_y; ++y) {
        for (std::size_t x = 0; x < res_x; ++x) {
            vec3 position = x * x_increment + y * y_increment + constant_axis;
            std::string object = "no_object";
            std::string material = "void";
            //backwards search objects
            for (int i = env.objects.size() - 1; i >= 0; --i) {
                if (env.objects[i].geom.point_inside(position)) {
                    object = env.objects[i].id;
                    material = env.material_ids[env.objects[i].material_id];
                    break;
                }
            }
            output_file << position[0] << "," << position[1] << "," << position[2] << ",";
            output_file << material << "," << object << "\n";
        }
    }

    output_file.close();
    return;
}

} // namespace projector