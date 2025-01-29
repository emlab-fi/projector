#include "filter.hpp"

namespace projector {

bool filter::check_particle(const particle &p) const {
    return check_particle_index(p, p.history.interactions.size() - 1);
}

bool filter::check_particle_index(const particle &p, std::size_t index) const {

    // check source
    if (source_id) {
        if (p.source->id != *source_id) {
            return false;
        }
    }

    // check object
    if (object_id) {
        if (p.history.objects[index] == nullptr || p.history.objects[index]->id != *object_id) {
            return false;
        }
    }

    // check interaction
    if (interaction) {
        if (p.history.interactions[index] != *interaction) {
            return false;
        }
    }

    // check element
    if (element) {
        if (p.history.elements[index] != *element) {
            return false;
        }
    }

    // check range
    if (energy_range) {
        if (p.history.energies[index] < energy_range->first ||
            p.history.energies[index] > energy_range->second) {
            return false;
        }
    }

    return true;
}

} // namespace projector