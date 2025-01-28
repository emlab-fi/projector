#include "filter.hpp"

namespace projector {

bool filter::check_particle(const particle &p) const {

    // check source
    if (source_id) {
        if (p.source->id != *source_id) {
            return false;
        }
    }

    // check object
    if (object_id) {
        if (p.current_object() == nullptr || p.current_object()->id != *object_id) {
            return false;
        }
    }

    // check interaction
    if (interaction) {
        if (p.history.interactions.back() != *interaction) {
            return false;
        }
    }

    // check element
    if (element) {
        if (p.history.elements.back() != *element) {
            return false;
        }
    }

    // check range
    if (energy_range) {
        if (p.energy() < energy_range->first || p.energy() > energy_range->second) {
            return false;
        }
    }

    return true;
}

} // namespace projector