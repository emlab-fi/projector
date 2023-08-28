#pragma once
#include "particle.hpp"

#include <filesystem>
#include <vector>
#include <variant>
#include <optional>

namespace projector {

/// @brief Abstract class representing the interface for different tally types.
///
/// This class acts as an interface for different tallies and as such has no implementation
/// whatsoever. Any derived class should only have the public methods that are virtual
/// in this class, no other public methods.
struct tally {

    virtual ~tally() = default;

    /// Initialize the tally, should be called only once at the beginning.
    virtual void init_tally() = 0;

    /// Add single particle data to the tally results.
    ///
    /// @param p The particle to add
    ///
    virtual void add_particle(const particle &p) = 0;

    /// Finalize the resulting data. Should be called only once at the end.
    ///
    virtual void finalize_data() = 0;

    /// Save tally results to filesystem.
    ///
    /// @param path Path for the tally. Should also include the filename.
    ///
    virtual void save_tally(const std::filesystem::path path) const = 0;
};

/// @brief The various scores (physical quantities) that can be evaluated.
///
/// Not all scores are supported by all tally types!
enum class tally_score {
    /// the total flux of the particles
    flux,
    /// average energy of a particle
    average_energy,
    /// total interaction counts
    interaction_counts
};

/// @brief Basic uniform mesh tally. Divides space into uniform grid.
///
/// The tally is defined by start and stop points and the resolution of the grid to divide the
/// measured volume into. It supports these scores: flux, average_energy, interaction_counts
class uniform_mesh_tally : public tally {

    bounding_box bounds; /// the bounds of the tally

    coord3 resolution; /// resolution for each axis

    std::size_t stride; /// the number of data per grid cell

    /// Stored data. Linear storage in vector.
    // Indexed so that coordinate x,y,z maps to index i:
    //     i = z * (resolution.x() * resolution.y()) + y * resolution.x() + x
    // Stored data is either double or int, depending on the score
    std::vector<std::variant<double, int>> data;

    tally_score score;

    /// Determine cell coordinates of a point
    /// @param point the point to evaluate
    /// @return returns empty if point is outside the grid, otherwise returns coordinates
    ///
    std::optional<coord3> determine_cell(const vec3& point) const;

    /// Calculate the base index for given coordinate
    /// @param coord the coordinate to evaluate
    /// @return the base index for given coordinate
    ///
    std::size_t calculate_index(const coord3& c) const;

    /// Calculate intersection values for all grid crossings of a segment
    /// @param start start point of the segment
    /// @param end end point of a segment
    /// @return interpolation coefficients for all grid crossings
    std::vector<double> calculate_intersections(const vec3 &start, const vec3& end) const;

    /// Add particle interactions to tally (tally must be have interaction count score setup)
    /// @param p particle to add
    void add_particle_interactionwise(const particle &p);

    /// Add particle segments to tally
    /// @param p particle to add
    void add_particle_segmentwise(const particle &p);

public:

    /// Default and only constructor for uniform_mesh_tally
    /// @param start The start point of the tally space
    /// @param end  The end point of the tally space
    /// @param res Resolution (number of cells) per axis
    /// @param sc  The score to evaluate
    ///
    uniform_mesh_tally(const vec3& start, const vec3& end, const coord3& res, tally_score sc);

    void init_tally() final;

    void add_particle(const particle &p) final;

    void finalize_data() final;

    void save_tally(const std::filesystem::path path) const final;
};

} // namespace projector