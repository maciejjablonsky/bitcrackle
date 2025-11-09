#include "math/statistics.h"
#include "math/waves.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <math/qnumber.h>
#include <numbers>
#include <print>
#include <ranges>

TEST_CASE("cordic::sine() to std::sin() correlation")
{
    std::vector<float> radians(10000);
    bit::linear_space(std::begin(radians),
                      std::end(radians),
                      -2 * std::numbers::pi,
                      2 * std::numbers::pi);
    auto std_sine =
        std::views::transform([](float angle) { return std::sin(angle); });
    auto cordic_sine = std::views::transform([](float angle) {
        auto argument = bit::qs<5, 25>{angle};
        return bit::cordic::sine<decltype(argument)>(argument).as<float>();
    });
    auto correlation =
        bit::pearson_correlation(radians | std_sine, radians | cordic_sine);
    std::println("Correlation: {}", correlation.value());
    CHECK(correlation.value() == Catch::Approx(1.f).epsilon(1e-2));
}
