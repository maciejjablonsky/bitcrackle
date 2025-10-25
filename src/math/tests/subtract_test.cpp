#include "math/qnumber.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("lowest() unsigned qnumber - max() unsigned qnumber")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::lowest();
    constexpr auto b = std::numeric_limits<bit::qu<3, 2>>::max();
    auto result      = a.subtract(b);
    CHECK(result.is_nearest_to(-7.75f));
}

TEST_CASE("lowest() signed qnumber - max() unsigned qnumber")
{
    constexpr auto a = std::numeric_limits<bit::qs<3, 2>>::lowest();
    constexpr auto b = std::numeric_limits<bit::qu<3, 2>>::max();
    auto result      = a.subtract(b);
    CHECK(result.is_nearest_to(-15.75f));
}
