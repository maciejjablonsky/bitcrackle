#include <catch2/catch_test_macros.hpp>
#include <math/qnumber.h>

TEST_CASE("accurate: max() unsigned qformat / max() same unsigned qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = std::numeric_limits<bit::qu<3, 2>>::max();
    auto result      = a.accurate_divide(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("saturate: max() unsigned qformat / max() same unsigned qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = std::numeric_limits<bit::qu<3, 2>>::max();
    auto result      = a.saturate_divide<bit::qu<3, 1>>(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("accurate: max() unsigned qformat / -1 * max() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<3, 2>>::max();
    auto result      = a.accurate_divide(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("saturate: max() unsigned qformat / -1 * max() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<3, 2>>::max();
    auto result      = a.saturate_divide<bit::qs<3, 1>>(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("accurate: max() unsigned qformat / -1 * lowest() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<2, 4>>::lowest();
    auto result      = a.accurate_divide(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("accurate: max() unsigned qformat / -1 * min() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<2, 4>>::min();
    auto result      = a.accurate_divide(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("accurate: lowest() signed qformat / max() unsigned qformat")
{
    constexpr auto a = std::numeric_limits<bit::qs<2, 4>>::lowest();
    constexpr auto b = std::numeric_limits<bit::qu<9, 2>>::max();
    auto result      = a.accurate_divide(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}
