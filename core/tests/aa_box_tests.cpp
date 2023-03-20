#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "volume.hpp"
#include "path.hpp"

void compare_vec3(const projector::vec3& a, const projector::vec3& b) {
    using Catch::Matchers::WithinRel;
    REQUIRE_THAT(a[0], WithinRel(b[0]));
    REQUIRE_THAT(a[1], WithinRel(b[1]));
    REQUIRE_THAT(a[2], WithinRel(b[2]));
}

TEST_CASE("AA Box basic calculations") {
    using namespace projector;

    SECTION("Fullfils concept generic_volume") {
        REQUIRE(generic_volume<AA_box>);
    }

    AA_box test_volume{{-1, -1, -1}, {1, 1, 1}};

    SECTION("Size calculation correct") {
        vec3 size = test_volume.size();
        compare_vec3(size, {2, 2, 2});
    }

    SECTION("Bounding box correct") {
        std::pair<vec3, vec3> bb = test_volume.bounding_box();
        compare_vec3(bb.first, {-1, -1, -1});
        compare_vec3(bb.second, {1, 1, 1});
    }
}

TEST_CASE("AA box intersections") {
    using namespace projector;

    AA_box test_volume{{-1, -1, -1}, {1, 1, 1}};

    SECTION("Empty intersections") {
    }

    SECTION("X axis intersection") {
        segment seg{{-2, 0, 0}, {1, 0, 0}, 4.0};
        auto res = test_volume.intersection(seg);
        REQUIRE(res.has_value());
        compare_vec3(res->start, {-1, 0, 0});
        compare_vec3(res->end_point(), {1, 0, 0});
    }

    SECTION("Y axis intersection") {
        segment seg{{0, 2, 0}, {0, -1, 0}, 4.0};
        auto res = test_volume.intersection(seg);
        REQUIRE(res.has_value());
        compare_vec3(res->start, {0, -1, 0});
        compare_vec3(res->end_point(), {0, 1, 0});
    }

    SECTION("Z axis intersection") {
        segment seg{{0, 0, -2.0}, {0, 0, 1}, 4.0};
        auto res = test_volume.intersection(seg);
        REQUIRE(res.has_value());
        compare_vec3(res->start, {0, 0, -1});
        compare_vec3(res->end_point(), {0, 0, 1});
    }

    SECTION("XY axis intersection") {

    }

    SECTION("XZ axis intersection") {

    }

    SECTION("YZ axis intersection") {

    }

    SECTION("XYZ axis intersection") {

    }

}