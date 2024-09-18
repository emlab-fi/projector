#pragma once
#include "geometry.hpp"
#include "material.hpp"
#include "particle.hpp"

#include <optional>

namespace projector {

/// @brief Tally filter class. Holds all possible filters and does the evaluation in correct order.
///
/// Holds all possible filters and does the evaluation in correct order.
/// Correct order is:
///    1. particle source ID
///    2. geometry ID
///    3. interaction type
///    4. interaction element
///    5. particle energy range
/// Can evaluate the whole particle (for volume and scintillator tally) or a single segment (for
/// uniform mesh tally). If the geometry is not specified, the whole simulation volume is counted
/// as the geometry.
struct filter {

    std::optional<std::size_t> source_id;
    const geometry *const geom;
    std::optional<cross_section> interaction;
    std::optional<std::size_t> element;
    std::optional<std::pair<double, double>> energy_range;

    /// Check whether particle passes through the filters.
    /// @param p The particle to check
    /// @return Whether the particle passed (true) or not (false)
    bool check_particle(const particle &p) const;

    /// Check whether particle segment passes through the filters.
    /// @param p particle to check
    /// @param segment index of segment to check
    /// @param position detailed position inside the index
    /// @return whether the segment passed (true) or not (false)
    bool check_particle_segment(const particle &p, const std::size_t segment,
                                const vec3 position) const;
};

} // namespace projector