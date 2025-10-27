#include <catch2/catch_test_macros.hpp>
#include <math/qnumber.h>

TEST_CASE("accurate: max() unsigned qformat * max() same unsigned qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = std::numeric_limits<bit::qu<3, 2>>::max();
    auto result      = a.accurate_multiply(b);
    CHECK(result.is_nearest_to(60.0625f));
}

TEST_CASE("saturate: max() unsigned qformat * max() same unsigned qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = std::numeric_limits<bit::qu<3, 2>>::max();
    auto result      = a.saturate_multiply<bit::qu<3, 1>>(b);
    CHECK(result.is_nearest_to(60.0625f));
}

TEST_CASE("accurate: max() unsigned qformat * -1 * max() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<3, 2>>::max();
    auto result      = a.accurate_multiply(b);
    CHECK(result.is_nearest_to(-60.0625f));
}

TEST_CASE("saturate: max() unsigned qformat * -1 * max() signed qformat")
{
    constexpr auto a          = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b          = -std::numeric_limits<bit::qs<3, 2>>::max();
    auto result               = a.saturate_multiply<bit::qs<3, 1>>(b);
    [[maybe_unused]] auto r_f = result.as<float>();
    CHECK(result.is_nearest_to(-60.0625f));
}
