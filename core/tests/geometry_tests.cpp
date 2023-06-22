#include <catch2/catch_test_macros.hpp>
#include "geometry.hpp"

TEST_CASE("Basic x plane") {
    projector::geom_primitive plane = {
        .type = projector::geom_primitive::shape::plane,
        .param1 = {0.0, 0.0, 0.0},
        .param2 = {1.0, 0.0, 0.0}
    };

    REQUIRE(plane.point_is_inside({-3.0, 0.0, 0.0}));
    REQUIRE_FALSE(plane.point_is_inside({3.0, 0.0, 0.0}));
    REQUIRE(plane.point_is_inside({-3.0, 2.5, -2.3}));
    REQUIRE_FALSE(plane.point_is_inside({3.0, -2.5, 2.3}));
}

TEST_CASE("Basic y plane") {
    projector::geom_primitive plane = {
        .type = projector::geom_primitive::shape::plane,
        .param1 = {0.0, 0.0, 0.0},
        .param2 = {0.0, 1.0, 0.0}
    };

    REQUIRE(plane.point_is_inside({0.0, -3.0, 0.0}));
    REQUIRE_FALSE(plane.point_is_inside({0.0, 3.0, 0.0}));
    REQUIRE_FALSE(plane.point_is_inside({-3.0, 2.5, -2.3}));
    REQUIRE(plane.point_is_inside({3.0, -2.5, 2.3}));
}

TEST_CASE("Basic z plane") {
    projector::geom_primitive plane = {
        .type = projector::geom_primitive::shape::plane,
        .param1 = {0.0, 0.0, 0.0},
        .param2 = {0.0, 0.0, 1.0}
    };

    REQUIRE(plane.point_is_inside({0.0, 0.0, -3.0}));
    REQUIRE_FALSE(plane.point_is_inside({0.0, 0.0, 3.0}));
    REQUIRE(plane.point_is_inside({-3.0, 2.5, -2.3}));
    REQUIRE_FALSE(plane.point_is_inside({3.0, -2.5, 2.3}));
}

TEST_CASE("Offset plane") {
    projector::geom_primitive plane = {
        .type = projector::geom_primitive::shape::plane,
        .param1 = {1.0, 1.0, 1.0},
        .param2 = {0.707107, 0.0, 0.707107}
    };

    REQUIRE(plane.point_is_inside({0.0, 0.0, 0.0}));
    REQUIRE_FALSE(plane.point_is_inside({2.0, 2.0, 2.0}));
}

TEST_CASE("Basic axis aligned box") {
    projector::geom_primitive box = {
        .type = projector::geom_primitive::shape::aa_box,
        .param1 = {-1.0, -1.0, -1.0},
        .param2 = {1.0, 1.0, 1.0}
    };

    REQUIRE(box.point_is_inside({0.0, 0.0, 0.0}));
    REQUIRE(box.point_is_inside({0.5, 0.75, -0.85}));
    REQUIRE_FALSE(box.point_is_inside({2.0, 0.0, 0.0}));
    REQUIRE_FALSE(box.point_is_inside({0.0, 2.0, 0.0}));
    REQUIRE_FALSE(box.point_is_inside({0.0, 0.0, 2.0}));
    REQUIRE_FALSE(box.point_is_inside({2.5, -3.5, 1.0}));
}

TEST_CASE("Basic axis aligned ellipsoid") {
    projector::geom_primitive ellipsoid = {
        .type = projector::geom_primitive::shape::aa_ellipsoid,
        .param1 = {0.0, 0.0, 0.0},
        .param2 = {1.0, 2.0, 3.0}
    };

    // TODO
}

TEST_CASE("Offset axis aligned ellipsoid") {
    projector::geom_primitive ellipsoid = {
        .type = projector::geom_primitive::shape::aa_ellipsoid,
        .param1 = {3.0, 2.0, 1.0},
        .param2 = {1.0, 2.0, 3.0}
    };

    // TODO
}

TEST_CASE("Operations") {
    projector::geom_primitive box1 = {
        .type = projector::geom_primitive::shape::aa_box,
        .param1 = {-1.0, -1.0, -1.0},
        .param2 = {1.0, 1.0, 1.0}
    };

    projector::geom_primitive box2 = {
        .type = projector::geom_primitive::shape::aa_box,
        .param1 = {0.0, 0.0, 0.0},
        .param2 = {2.0, 2.0, 2.0}
    };

    SECTION("join") {
        projector::operation op;
        op.op = projector::operation::type::join;
        op.left = std::make_unique<projector::geometry>(box1);
        op.right = std::make_unique<projector::geometry>(box2);

        REQUIRE(op.point_is_inside({-0.5, -0.5, -0.5}));
        REQUIRE(op.point_is_inside({0.0, 0.0, 0.0}));
        REQUIRE(op.point_is_inside({1.5, 1.5, 1.5}));
        REQUIRE_FALSE(op.point_is_inside({-3.0, -3.0, -3.0}));
        REQUIRE_FALSE(op.point_is_inside({3.0, 3.0, 3.0}));
        REQUIRE_FALSE(op.point_is_inside({-1.0, -1.0, -2.0}));
    }

    SECTION("intersect") {
        projector::operation op;
        op.op = projector::operation::type::intersect;
        op.left = std::make_unique<projector::geometry>(box1);
        op.right = std::make_unique<projector::geometry>(box2);

        REQUIRE_FALSE(op.point_is_inside({-0.5, -0.5, -0.5}));
        REQUIRE(op.point_is_inside({0.5, 0.5, 0.5}));
        REQUIRE_FALSE(op.point_is_inside({1.5, 1.5, 1.5}));
        REQUIRE_FALSE(op.point_is_inside({-3.0, -3.0, -3.0}));
        REQUIRE_FALSE(op.point_is_inside({3.0, 3.0, 3.0}));
        REQUIRE_FALSE(op.point_is_inside({-1.0, -1.0, -2.0}));
    }

    SECTION("cut") {
        projector::operation op;
        op.op = projector::operation::type::cut;
        op.left = std::make_unique<projector::geometry>(box1);
        op.right = std::make_unique<projector::geometry>(box2);

        REQUIRE(op.point_is_inside({-0.5, -0.5, -0.5}));
        REQUIRE(op.point_is_inside({0.5, 0.5, -0.5}));
        REQUIRE(op.point_is_inside({0.0, 0.0, 0.0}));
        REQUIRE_FALSE(op.point_is_inside({1.5, 1.5, 1.5}));
        REQUIRE_FALSE(op.point_is_inside({-3.0, -3.0, -3.0}));
        REQUIRE_FALSE(op.point_is_inside({3.0, 3.0, 3.0}));
        REQUIRE_FALSE(op.point_is_inside({-1.0, -1.0, -2.0}));
    }
}

TEST_CASE("Basic geometry tree") {

}
