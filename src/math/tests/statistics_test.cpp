#include "math/statistics.h"
#include <catch2/catch_test_macros.hpp>

#include <catch2/catch_approx.hpp>

template <typename T, std::integral IndexT>
    requires std::convertible_to<IndexT, T>
[[nodiscard]] constexpr auto natural_numbers(IndexT start, IndexT end) noexcept
{
    return std::views::iota(start, end) |
           std::views::transform(
               [](std::integral auto value) { return static_cast<T>(value); });
}

TEST_CASE("Completely correlated sets have pearson_correlation == 1")
{
    auto coefficient = bit::pearson_correlation(
        natural_numbers<float>(0, 100), natural_numbers<double>(0, 100));
    REQUIRE(coefficient.has_value());
    REQUIRE(coefficient.value() == Catch::Approx(1.f));
}

TEST_CASE("Completely uncorrelated sets have pearson_correlation == 0")
{
    auto natural = natural_numbers<double>(0, 1000);
    auto sine_of_natural =
        natural | std::views::transform([](std::floating_point auto value) {
            return std::sin(value);
        });
    auto coefficient = bit::pearson_correlation(natural, sine_of_natural);
    REQUIRE(coefficient.has_value());
    REQUIRE(coefficient.value() == Catch::Approx(0.f).margin(1e-2));
}

TEST_CASE("Completely opposite sets have pearson_correlation == -1")
{
    auto natural = natural_numbers<double>(0, 100);
    auto negative_natural =
        natural_numbers<float>(0, 100) |
        std::views::transform(
            [](std::floating_point auto value) { return -value; });
    auto coefficient = bit::pearson_correlation(negative_natural, natural);
    REQUIRE(coefficient.has_value());
    REQUIRE(coefficient.value() == Catch::Approx(-1.f));
}
