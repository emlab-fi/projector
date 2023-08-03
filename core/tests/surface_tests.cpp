#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "surface.hpp"

TEST_CASE("Axis aligned plane tests") {

    projector::plane plane({0.0, 0.0, 0.0}, {0.0, 0.0, 1.0});

}

TEST_CASE("Generic plane") {

    projector::plane plane({1.0, 1.0, 1.0}, {-1.0, -1.0, -1.0});

}

TEST_CASE("Ellipsoid") {

    projector::ellipsoid ellip({1.0, 1.0, 1.0}, 1.0, 2.0, 3.0);

}

TEST_CASE("X cylinder") {

    projector::x_cylinder cyl({1.0, 1.0, 1.0}, 1.0, 2.0);

}

TEST_CASE("Y cylinder") {

    projector::y_cylinder cyl({1.0, 1.0, 1.0}, 1.0, 2.0);

}

TEST_CASE("Z cylinder") {

    projector::z_cylinder cyl({1.0, 1.0, 1.0}, 1.0, 2.0);

}

TEST_CASE("X cone") {

    projector::x_cone cone({1.0, 1.0, 1.0}, 3.0, 2.0, 1.0);

}

TEST_CASE("Y cone") {

    projector::y_cone cone({1.0, 1.0, 1.0}, 3.0, 2.0, 1.0);

}

TEST_CASE("Z cone") {

    projector::y_cone cone({1.0, 1.0, 1.0}, 3.0, 2.0, 1.0);

}