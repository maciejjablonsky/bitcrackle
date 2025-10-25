#include <catch2/catch_test_macros.hpp>
#include <math/qnumber.h>

TEST_CASE("Add signed qnumbers with matching integer and fraction bits",
          "[float2fixed]")
{
    constexpr bit::qs<3, 2> a{5.25f};
    constexpr bit::qs<3, 2> b{4.5f};
    constexpr auto result = a.add(b);
    static_assert(result.is_nearest_to(5.25f + 4.5f));
}

TEST_CASE("Add signed qnumbers with different integer and same fraction bits",
          "[float2fixed]")
{
    constexpr bit::qs<3, 2> a{5.25f};
    constexpr bit::qs<4, 2> b{15.5f};
    constexpr auto result = a.add(b);
    static_assert(result.is_nearest_to(5.25f + 15.5f));
}

TEST_CASE("Add signed and unsigned qnumbers with different integer bits and "
          "same fraction bits",
          "[float2fixed]")
{
    constexpr bit::qs<3, 2> a{-5.25f};
    constexpr bit::qu<4, 2> b{15.5f};
    constexpr auto result = a.add(b);
    static_assert(result.is_nearest_to(-5.25f + 15.5f));
}

TEST_CASE("Add signed and unsigned qnumbers with matching integer bits and "
          "fraction bits",
          "[float2fixed]")
{
    constexpr bit::qs<3, 2> a{-5.25f};
    constexpr bit::qu<3, 2> b{4.5f};
    constexpr auto result = a.add(b);
    static_assert(result.is_nearest_to(-5.25f + 4.5f));
}

TEST_CASE("Add two max unsigned qnumbers")
{
    auto a      = std::numeric_limits<bit::qu<3, 2>>::max();
    auto b      = std::numeric_limits<bit::qu<4, 2>>::max();
    auto result = a.add(b);
    static_assert(decltype(result)::bits == 7);
}

TEST_CASE("Add two max signed qnumbers")
{
    auto a      = std::numeric_limits<bit::qs<3, 2>>::max();
    auto b      = std::numeric_limits<bit::qs<4, 2>>::max();
    auto result = a.add(b);
    static_assert(decltype(result)::bits == 7);
}

TEST_CASE("Add lowest signed qnumber to max unsigned qnumber")
{
    auto a      = std::numeric_limits<bit::qu<3, 2>>::max();
    auto b      = std::numeric_limits<bit::qs<0, 2>>::lowest();
    auto result = a.add(b);
    CHECK(result.is_nearest_to(6.75f));
}

TEST_CASE("Add lowest signed qnumber to unsigned max qnumber")
{
    auto a           = std::numeric_limits<bit::qu<3, 2>>::max();    // 31
    constexpr auto b = std::numeric_limits<bit::qs<3, 2>>::lowest(); // -32
    auto result      = a.add(b);
    CHECK(result.is_nearest_to(-0.25f));
}

TEST_CASE("Add lowest signed qnumber to other lowest signed qnumber")
{
    constexpr auto a = std::numeric_limits<bit::qs<42, 2>>::lowest();
    constexpr auto b = std::numeric_limits<bit::qs<60, 2>>::lowest();
    auto result      = a.add(b);
    CHECK(result.is_nearest_to(-1152925902653358080.L));
}
