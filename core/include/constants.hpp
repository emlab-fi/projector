#pragma once
#include <numbers>
#include <limits>

namespace projector::constants {

/// value of PI
constexpr double pi = std::numbers::pi_v<double>;

// these constants are taken from openmc and
// https://physics.nist.gov/cuu/Constants/

/// electron mass in ev/c^2
constexpr double electron_mass_ev = 0.51099895000e6;

/// planck constant times c in eV-Angstroms
constexpr double planck_c = 1.2398419839593942e4;

/// avogadro constant
constexpr double avogadro = 6.02214076e23;

/// infinity value
constexpr double infinity = std::numeric_limits<double>::infinity();

constexpr double epsilon = std::numeric_limits<double>::epsilon();

} // namespace projector::constants