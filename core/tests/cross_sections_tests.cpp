#include <catch2/catch_test_macros.hpp>
#include "cross_sections.hpp"

namespace {

void assert_element(std::pair<std::string_view, int>& elem, const std::string_view& expected, int expected_count) {
    REQUIRE(elem.first.compare(expected));
    REQUIRE(elem.second == expected_count);
}

} //annonymous namespace

TEST_CASE("Basic formula parsing") {
    SECTION("Single letter symbol") {
        auto res1 = projector::parse_material_string("I");
        assert_element(res1[0], "I", 1);

        auto res2 = projector::parse_material_string("O2");
        assert_element(res2[0], "O", 2);

        auto res3 = projector::parse_material_string("H20");
        assert_element(res3[0], "H", 20);
    }

    SECTION("Double letter symbol") {
        auto res1 = projector::parse_material_string("Be");
        assert_element(res1[0], "Be", 1);

        auto res2 = projector::parse_material_string("Fe2");
        assert_element(res2[0], "Fe", 2);

        auto res3 = projector::parse_material_string("Au20");
        assert_element(res3[0], "Au", 20);
    }
}

TEST_CASE("Compound formula parsing") {
    SECTION("Single letter symbols") {
        auto res1 = projector::parse_material_string("KO");
        assert_element(res1[0], "K", 1);
        assert_element(res1[1], "O", 1);

        auto res2 = projector::parse_material_string("H2O");
        assert_element(res2[0], "H", 2);
        assert_element(res2[1], "O", 1);

        auto res3 = projector::parse_material_string("C3H12");
        assert_element(res3[0], "C", 3);
        assert_element(res3[1], "H", 12);

    }

    SECTION("Double letter symbols") {
        auto res1 = projector::parse_material_string("LiMn");
        assert_element(res1[0], "Li", 1);
        assert_element(res1[1], "Mn", 1);

        auto res2 = projector::parse_material_string("Fe3Au2");
        assert_element(res2[0], "Fe", 3);
        assert_element(res2[1], "Au", 2);
    }

    SECTION("Combined symbols") {
        auto res = projector::parse_material_string("Fe3O2Pb12C3");
        assert_element(res[0], "Fe", 3);
        assert_element(res[1], "O", 2);
        assert_element(res[2], "Pb", 12);
        assert_element(res[3], "C", 3);
    }
}