#include "filter.hpp"

namespace projector {

bool energy_range_filter::check_particle(const particle& p) {
    return (p.energy() >= energy_low && p.energy() <= energy_high);
}

bool energy_range_filter::check_particle_segment(const particle& p, const std::size_t index) {
    double e = p.history.energies[index];
    return (e >= energy_low && e <= energy_high);
}

bool element_filter::check_particle(const particle& p) {
    return p.history.elements.back() == element;
}

bool element_filter::check_particle_segment(const particle& p, const std::size_t index) {
    return p.history.elements[index] == element;
}

bool interaction_filter::check_particle(const particle& p) {
    return p.history.interactions.back() == interaction;
}

bool interaction_filter::check_particle_segment(const particle& p, const std::size_t index) {
    return p.history.interactions[index] == interaction;
}

bool geometry_filter::check_particle(const particle& p) {
    for (std::size_t i = 0; i < p.history.points.size(); ++i) {
        if (check_particle_segment(p, i)) {
            return true;
        }
    }
    return false;
}

bool geometry_filter::check_particle_segment(const particle& p, const std::size_t index) {
    return geom.point_inside(p.history.points[index]);
}


} // namespace projector