#pragma once
#include "environment.hpp"

#include <filesystem>

namespace projector {

/// Plot a 2D slice of geometry to a CSV file.
///
/// @param env The environment to plot
/// @param plane The plane of the slice - can be one of "x", "y" or "z"
/// @param center The location of the plane on the axis
/// @param res_x Resolution of the output in the X axis
/// @param res_y Resolution of the output in the Y axis
/// @param output Output path of the CSV file
void env_slice_plot(const environment &env, const char plane, double center,
                    std::size_t res_x, std::size_t res_y, const std::filesystem::path output);

} // namespace projector