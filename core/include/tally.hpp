#pragma once
#include "filter.hpp"
#include "particle.hpp"

#include <fstream>
#include <vector>

namespace projector {

/// The various scores (physical quantities) that can be evaluated.
enum class tally_score {
    /// the total flux of the particles
    flux,
    /// average energy of a particle
    average_energy,
    /// total interaction counts
    interaction_counts,
    /// energy deposited by particles
    deposited_energy
};

/// @brief Abstract class representing the interface for different tally types.
///
/// This class acts as an interface for different tallies and as such has no implementation
/// whatsoever. Any derived class should only have the public methods that are virtual
/// in this class, no other public methods.
struct tally {

    virtual ~tally() = default;

    /// Initialize the tally, should be called only once at the beginning.
    ///
    /// @param data Pointer to data to initialze.
    ///
    virtual void init_tally(std::vector<double> *data) = 0;

    /// Add single particle data to the tally results.
    ///
    /// @param p The particle to add
    /// @param filters The particle filters to use
    ///
    virtual void add_particle(const particle &p,
                              const std::vector<std::unique_ptr<filter>> &filters) = 0;

    /// Finalize the resulting data. Should be called only once at the end.
    virtual void finalize_data() = 0;

    /// Save the final tally data. Expects filename and the final data from manager.
    /// @param output The file output stream to save to
    /// @param final_mean The final calculated mean values
    /// @param final_variance The final calculated variance values
    virtual void save_tally(std::fstream &output, std::vector<double> &mean,
                            std::vector<double> &variance) = 0;
};

/// @brief Basic volume tally. Counts score on the whole simulation volume.
class volume_tally : public tally {

    tally_score score; /// the tally score to count

    std::vector<double> *data_storage; /// pointer to current storage

public:
    /// Default and only constructor.
    /// @param sc The tally score to count
    ///
    volume_tally(tally_score sc);

    void init_tally(std::vector<double> *data) final;

    void add_particle(const particle &p, const std::vector<std::unique_ptr<filter>> &filters) final;

    void finalize_data() final;

    void save_tally(std::fstream &output, std::vector<double> &mean,
                    std::vector<double> &variance) final;
};

/// @brief Uniform mesh tally. Divides space into uniform grid.
///
/// The tally is defined by start and stop points and the resolution of the grid to divide the
/// measured volume into. Data stored lineary in vector in doubles.
/// Indexed so that coordinate x,y,z maps to index i:
// /    i = z * (resolution.x() * resolution.y()) + y * resolution.x() + x
class uniform_mesh_tally : public tally {

    bounding_box bounds; /// the bounds of the tally

    coord3 resolution; /// resolution for each axis

    std::size_t stride; /// the number of data per grid cell

    tally_score score; /// the tally score to count

    std::vector<double> *data_storage; /// pointer to current storage

    /// Determine cell coordinates of a point
    /// @param point the point to evaluate
    /// @return returns empty if point is outside the grid, otherwise returns coordinates
    ///
    std::optional<coord3> determine_cell(const vec3 &point) const;

    /// Calculate the base index for given coordinate
    /// @param coord the coordinate to evaluate
    /// @return the base index for given coordinate
    ///
    std::size_t calculate_index(const coord3 &c) const;

    /// Calculate intersection values for all grid crossings of a segment
    /// @param start start point of the segment
    /// @param end end point of a segment
    /// @return interpolation coefficients for all grid crossings
    std::vector<double> calculate_intersections(const vec3 &start, const vec3 &end) const;

    /// Increment the data by one at a given index
    /// @param index index to increment
    void increment_index(std::size_t index);

    /// Add value at a given index
    /// @param index index to update
    /// @param value value to add to index
    void add_index(std::size_t index, double value);

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
    uniform_mesh_tally(const vec3 &start, const vec3 &end, const coord3 &res,
                       tally_score sc);

    void init_tally(std::vector<double> *data) final;

    void add_particle(const particle &p, const std::vector<std::unique_ptr<filter>> &filters) final;

    void finalize_data() final;

    void save_tally(std::fstream &output, std::vector<double> &mean,
                    std::vector<double> &variance) final;
};

/// @brief Energy histogram tally.
///
/// This tally creates an energy histogram of the particles under provided filters.
/// The histogram has specified number of bins, particles with energy larger than max_energy are
/// counted into the highest bin. The bin edges are >= && <
class histogram_tally : public tally {

    std::size_t bins; /// number of bins to divide particles into

    double max_energy; /// largest

    std::vector<double> *data_storage; /// pointer to current storage

public:
    /// Default and only constructor.
    /// @param bins number of energy bins to count
    /// @param max_energy maximum energy (highest bin energy)
    histogram_tally(std::size_t bins, double max_energy);

    void init_tally(std::vector<double> *data) final;

    void add_particle(const particle &p, const std::vector<std::unique_ptr<filter>> &filters) final;

    void finalize_data() final;

    void save_tally(std::fstream &output, std::vector<double> &mean,
                    std::vector<double> &variance) final;
};

/// @brief Tally manager for multiple batches of a single tally.
///
/// This class represents a single tally in the environment through all batches.
/// It manages the data storage for the tally for each batch run.
/// The tally calculation itself is delegated to an external tally class.
class tally_manager {
    std::string id; /// user supplied ID

    std::vector<std::vector<double>> data; /// main data storage
    std::vector<double> final_mean; /// final calculated mean values
    std::vector<double> final_variance; /// final calculated variance values

    std::unique_ptr<tally> tally_type; /// the managed tally
    std::vector<std::unique_ptr<filter>> filters; /// the filters to use with the tally
public:

    tally_manager(std::string usr_id, std::unique_ptr<tally> tally_type,
                  std::vector<std::unique_ptr<filter>> filters);

    /// Initialize a new batch, should be called after previous batch was finalized.
    void init_batch();

    /// Add single particle data to the tally results of current batch.
    ///
    /// @param p The particle to add
    ///
    void add_particle(const particle &p);

    /// Finalize the batch, should be called only once per batch
    void finalize_batch();

    /// Finalize the resulting data. Should be called only once at the end.
    ///
    void finalize_data();

    /// Save tally results to filesystem.
    ///
    /// @param path Path for the tally. Should not include the filename!
    ///
    void save_tally(const std::filesystem::path path);
};

} // namespace projector