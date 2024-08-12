#pragma once
#include "geometry.hpp"
#include "material.hpp"
#include "particle.hpp"

namespace projector {

/// @brief Abstract class representing the interface of different filter types
///
/// This class is an interface to the filters used by the tallies and has no implementation.
/// Any derived class should only have the  methods derived from this interface public. Other
/// methods should be private.
struct filter {

    virtual ~filter() = default;

    /// Check the particle whether it passes the filter. Behaviour depends on implementation class.
    ///
    /// @param p the particle to check
    /// @return true if particle passed, false otherwise
    ///
    virtual bool check_particle(const particle &p) = 0;

    /// Check whether specific segment of particle path passes the filter.
    /// Behaviour depends on implementation class.
    ///
    /// @param p the particle to check
    /// @param segment_index index of the particle's segment to check
    /// @return true if segment passed, false otherwise
    ///
    virtual bool check_particle_segment(const particle &p, const std::size_t segment_index) = 0;
};


class energy_range_filter : public filter {
    const double energy_low;
    const double energy_high;

public:
    energy_range_filter(double low, double high) : energy_low(low), energy_high(high) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class element_filter : public filter {
    const std::size_t element;

public:
    element_filter(std::size_t element) : element(element) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class interaction_filter : public filter {
    const cross_section interaction;

public:
    interaction_filter(cross_section interaction) : interaction(interaction) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class geometry_filter : public filter {
    const geometry &geom;

public:
    geometry_filter(const geometry &geom) : geom(geom) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class material_filter : public filter {
    const material_data &material;

public:
    material_filter(const material_data &material) : material(material) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class source_filter : public filter {
    const std::string src_id;

public:
    source_filter(std::string src_id) : src_id(src_id) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

} // namespace projector