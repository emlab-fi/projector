#pragma once
#include "geometry.hpp"
#include "material.hpp"
#include "particle.hpp"

namespace projector {

struct filter {
    virtual ~filter() = default;

    virtual bool check_particle(const particle &p) = 0;

    virtual bool check_particle_segment(const particle &p, const std::size_t segment_index) = 0;
};

class energy_range_filter : public filter {
    double energy_low;
    double energy_high;

public:
    energy_range_filter(double low, double high) : energy_low(low), energy_high(high) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class element_filter : public filter {
    std::size_t element;

public:
    element_filter(std::size_t element) : element(element) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class interaction_filter : public filter {
    cross_section interaction;

public:
    interaction_filter(cross_section interaction) : interaction(interaction) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class geometry_filter : public filter {
    geometry &geom;

public:
    geometry_filter(geometry &geom) : geom(geom) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class material_filter : public filter {
    material_data &material;

public:
    material_filter(material_data &material) : material(material) {}

    bool check_particle(const particle &p) final;

    bool check_particle_segment(const particle &p, const std::size_t segment_index) final;
};

class source_filter : public filter {};

} // namespace projector