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
///    2. object ID
///    3. interaction type
///    4. interaction element
///    5. particle energy range
/// Can evaluate the whole particle (for volume and scintillator tally) or a single segment (for
/// uniform mesh tally). If the geometry is not specified, the whole simulation volume is counted
/// as the geometry.
struct filter {

    std::optional<std::string> source_id;
    std::optional<std::string> object_id;
    std::optional<cross_section> interaction;
    std::optional<std::size_t> element;
    std::optional<std::pair<double, double>> energy_range;

    /// Check whether particle passes through the filters.
    /// @param p The particle to check
    /// @return Whether the particle passed (true) or not (false)
    bool check_particle(const particle &p) const;
};

} // namespace projector