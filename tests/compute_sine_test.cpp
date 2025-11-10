#include "math/utilities.h"
#include "math/waves.h"
#include "peep/peeper.h"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <numbers>
#include <print>
#include <ranges>

TEST_CASE("Positive 2 powers", "[float2fixed]")
{
    using namespace std::views;
    auto powers = iota(-10, 11) | transform([](std::integral auto value) {
                      return std::pow(2, value) - std::pow(2, -10);
                  });

    auto powers_in_fixed_point =
        powers | transform([](std::floating_point auto value) {
            return bit::qs<10, 10>{value};
        });

    bit::peep::peeper peeper;
    peeper.plot(powers);
    peeper.plot(powers_in_fixed_point);
    peeper.show();

    CHECK(std::ranges::equal(powers,
                             powers_in_fixed_point |
                                 transform([](bit::qformatted auto value) {
                                     return value.as<float>();
                                 })));
}

TEST_CASE("Negative 2 powers", "[float2fixed]")
{
    using namespace std::views;
    auto powers = iota(-10, 11) | transform([](std::integral auto value) {
                      return -(std::pow(2, value) - std::pow(2, -10));
                  });

    auto powers_in_fixed_point =
        powers | transform([](std::floating_point auto value) {
            return bit::qs<10, 10>(value);
        });

    CHECK(std::ranges::equal(powers,
                             powers_in_fixed_point |
                                 transform([](bit::qformatted auto value) {
                                     return value.as<float>();
                                 })));
}

TEST_CASE("Subtract signed qnumber from max signed qnumber", "[float2fixed]")
{
    constexpr bit::qs<3, 2> a{7.75f}; // max for qs<3,2>
    constexpr bit::qs<3, 2> b{1.5f};
    constexpr auto result = a - b;
    static_assert(result.is_nearest_to(7.75f - 1.5f));
}

TEST_CASE("Subtract signed qnumber from max unsigned qnumber", "[float2fixed]")
{
    constexpr bit::qu<4, 2> a{15.75f}; // max for qu<4,2>
    constexpr bit::qs<3, 2> b{2.25f};
    constexpr auto result = a - b;
    static_assert(result.is_nearest_to(15.75f - 2.25f));
}

TEST_CASE("Safely cast qnumber into wider fraction format", "[float2fixed]")
{
    constexpr bit::qu<4, 2> a{15.75f}; // max for qu<4,2>
    constexpr auto result = a.as<bit::qu<4, 3>>();
    static_assert(result.is_nearest_to(15.75f));
}

// TEST_CASE("Accurate multiply unsigned by unsigned")
// {
//     constexpr bit::qu<4, 2> a{15.75f}; // max for qu<4,2>
//     constexpr bit::qu<2, 2> b{2.25f};
//     auto result = a.accurate_mul(b);
//     CHECK(result.is_nearest_to(15.75f * 2.25f));
// }
//
// TEST_CASE("Accurate multiply signed by signed")
// {
//     constexpr bit::qs<4, 2> a{15.75f}; // max for qu<4,2>
//     constexpr bit::qs<2, 2> b{-2.25f};
//     auto result = a.accurate_mul(b);
//     CHECK(result.is_nearest_to(15.75f * -2.25f));
// }
//
// TEST_CASE("Accurate multiply signed by unsigned")
// {
//     constexpr bit::qs<4, 2> a{-15.75f}; // max for qu<4,2>
//     constexpr bit::qu<2, 2> b{2.25f};
//     auto result = a.accurate_mul(b);
//     CHECK(result.is_nearest_to(-15.75f * 2.25f));
// }
//
// TEST_CASE("Accurate multiply unsigned by signed")
// {
//     constexpr bit::qu<4, 2> a{15.75f}; // max for qu<4,2>
//     constexpr bit::qs<2, 2> b{-2.25f};
//     auto result = a.accurate_mul(b);
//     CHECK(result.is_nearest_to(15.75f * -2.25f));
// }
//
// TEST_CASE("Accurate multiply max unsigned fraction by max unsigned fraction")
// {
//     constexpr auto a      = std::numeric_limits<bit::qu<0, 32>>::max();
//     constexpr auto b      = std::numeric_limits<bit::qu<0, 32>>::max();
//     bit::qu<0, 64> result = a.accurate_mul(b);
//     CHECK(result.is_nearest_to(a.as<double>() * b.as<double>()));
// }
//
// TEST_CASE("Accurate multiply max signed fraction by max unsigned fraction")
// {
//     constexpr auto a      = std::numeric_limits<bit::qs<0, 31>>::max();
//     constexpr auto b      = std::numeric_limits<bit::qu<0, 32>>::max();
//     bit::qs<0, 63> result = a.accurate_mul(b);
//     CHECK(result.is_nearest_to(a.as<double>() * b.as<double>()));
// }
//
// TEST_CASE("Accurate multiply max unsigned fraction by max signed fraction")
// {
//     constexpr auto a      = std::numeric_limits<bit::qu<0, 32>>::max();
//     constexpr auto b      = std::numeric_limits<bit::qs<0, 31>>::max();
//     bit::qs<0, 63> result = a.accurate_mul(b);
//     CHECK(result.is_nearest_to(a.as<double>() * b.as<double>()));
// }
//
// TEST_CASE("Accurate multiply max signed fraction by max signed fraction")
// {
//     constexpr auto a      = std::numeric_limits<bit::qs<0, 32>>::max();
//     constexpr auto b      = std::numeric_limits<bit::qs<0, 31>>::max();
//     bit::qs<0, 63> result = a.accurate_mul(b);
//     CHECK(result.is_nearest_to(a.as<double>() * b.as<double>()));
// }

/*
TEST_CASE("Saturate multiply unsigned by unsigned")
{
    constexpr bit::qu<4, 2> a{15.75f}; // max for qu<4,2>
    constexpr bit::qu<2, 2> b{2.25f};
    auto result = a.saturate_mul<bit::qu<5, 2>>(b);
    CHECK(result.is_nearest_to(15.75f * 2.25f));
}

TEST_CASE("Saturate multiply signed by signed")
{
    constexpr bit::qs<4, 2> a{15.75f}; // max for qu<4,2>
    constexpr bit::qs<2, 2> b{-2.25f};
    auto result = a.saturate_mul<bit::qs<5, 2>>(b);
    CHECK(result.is_nearest_to(15.75f * -2.25f));
}

TEST_CASE("Saturate multiply signed by unsigned")
{
    constexpr bit::qs<4, 2> a{-15.75f}; // max for qu<4,2>
    constexpr bit::qu<2, 2> b{2.25f};
    auto result = a.saturate_mul<bit::qs<5, 2>>(b);
    CHECK(result.is_nearest_to(-15.75f * 2.25f));
}

TEST_CASE("Saturate multiply unsigned by signed")
{
    constexpr bit::qu<4, 2> a{15.75f}; // max for qu<4,2>
    constexpr bit::qs<2, 2> b{-2.25f};
    auto result = a.saturate_mul<bit::qs<5, 2>>(b);
    CHECK(result.is_nearest_to(15.75f * -2.25f));
}

TEST_CASE("Accurate divide unsigned by unsigned")
{
    constexpr bit::qu<4, 2> a{15.75f}; // max for qu<4,2>
    constexpr bit::qu<2, 2> b{3.0f};
    auto result  = a.accurate_div(b);
    CHECK(result.is_nearest_to(15.75f / 3.0f));
}

TEST_CASE("Accurate divide signed by unsigned")
{
    constexpr bit::qs<4, 2> a{-12.5f};
    constexpr bit::qu<2, 2> b{2.0f};
    auto result = a.accurate_div(b);
    CHECK(result.is_nearest_to(-12.5f / 2.0f));
}

TEST_CASE("Accurate divide unsigned by signed (positive)")
{
    constexpr bit::qu<4, 2> a{15.75f};
    constexpr bit::qs<3, 2> b{3.25f};
    auto result = a.accurate_div(b);
    CHECK(result.is_nearest_to(15.75f / 3.25f));
}

TEST_CASE("Accurate divide unsigned by signed (negative)")
{
    constexpr bit::qu<4, 2> a{15.75f};
    constexpr bit::qs<3, 2> b{-2.5f};
    auto result = a.accurate_div(b);
    CHECK(result.is_nearest_to(15.75f / -2.5f));
}

TEST_CASE("Accurate divide signed by signed")
{
    bit::qs<4, 1> a{-15.5f};
    bit::qs<2, 1> b{-3.5f};
    auto result = a.accurate_div(b);
    CHECK(result.is_nearest_to(-15.5f / -3.5f));
}

TEST_CASE("Divide unsigned integer by unsigned integer with fractional result")
{
    bit::qu<1, 1> a{1.f};
    bit::qu<2, 0> b{2.f};
    auto result = a.accurate_div(b);
    CHECK(result.is_nearest_to(1.f / 2.f));
}

TEST_CASE("Saturate divide unsigned by unsigned")
{
    constexpr bit::qu<4, 2> a{15.75f};  // max for qu<4,2>
    constexpr bit::qu<2, 2> b{2.25f};
    auto result = a.saturate_div<bit::qu<5,2>>(b); // note: qu<5,4> — enough
fraction bits CHECK(result.is_nearest_to(15.75f / 2.25f));
}

TEST_CASE("Saturate divide signed by signed")
{
    constexpr bit::qs<4, 2> a{15.75f};  // max for qs<4,2>
    constexpr bit::qs<2, 2> b{-2.25f};
    auto result = a.saturate_div<bit::qs<5,2>>(b);
    CHECK(result.is_nearest_to(15.75f / -2.25f));
}

TEST_CASE("Saturate divide signed by unsigned")
{
    constexpr bit::qs<4, 2> a{-15.75f};  // max for qs<4,2>
    constexpr bit::qu<2, 2> b{2.25f};
    auto result = a.saturate_div<bit::qs<5,2>>(b);
    CHECK(result.is_nearest_to(-15.75f / 2.25f));
}

TEST_CASE("Saturate divide unsigned by signed")
{
    constexpr bit::qu<4, 2> a{15.75f};  // max for qu<4,2>
    constexpr bit::qs<2, 2> b{-2.25f};
    auto result = a.saturate_div<bit::qs<5,2>>(b);
    CHECK(result.is_nearest_to(15.75f / -2.25f));
}
*/

// TEST_CASE("Accurate sine of known values")
// {
//     auto sine_of_zero = bit::accurate_sine(bit::qu<0,0>(0.f));
//     auto sine_of_zero_float = bit::float_sine(0);
//
//     auto sine_of_pi_thirds =
//     bit::accurate_sine(bit::qu<3,5>(std::numbers::pi_v<float> / 3.f)); auto
//     sine_of_pi_thirds_float = bit::float_sine(std::numbers::pi_v<float>
//     / 3.f);
//
//     auto sine_of_pi_twos = bit::accurate_sine(bit::qu<3,5>(std::numbers::pi
//     / 2.f)); auto sine_of_pi_twos_float = bit::float_sine(std::numbers::pi
//     / 2.f);
//
//     auto sine_of_pi = bit::accurate_sine(bit::qu<3,5>(std::numbers::pi));
//     auto sine_of_pi_float = bit::float_sine(std::numbers::pi);
//
//     auto sine_of_pi_and_thirds =
//     bit::accurate_sine(bit::qu<3,5>(std::numbers::pi + std::numbers::pi
//     / 3.f)); auto sine_of_pi_and_thirds_float =
//     bit::float_sine(std::numbers::pi + std::numbers::pi / 3.f);
//
//     auto sine_of_pi_and_twos =
//     bit::accurate_sine(bit::qu<3,5>(std::numbers::pi + std::numbers::pi
//     / 2.f)); auto sine_of_pi_and_twos_float =
//     bit::float_sine(std::numbers::pi + std::numbers::pi / 2.f);
//
//     auto sine_of_two_pi = bit::accurate_sine(bit::qu<3,5>(2.f *
//     std::numbers::pi)); auto sine_of_two_pi_float = bit::float_sine(2.f *
//     std::numbers::pi); CHECK(true);
// }

// TEST_CASE("Generate fixed point sine")
// {
//     auto angles  = std::views::iota(0, 361);
//     auto radians = angles | std::views::transform([](std::integral auto
//     degrees) {
//                        return degrees * std::numbers::pi / 180;
//                    });
//     std::ranges::for_each(radians, [](std::floating_point auto value) {
//         std::println("radians:: {}", value);
//     });
//     auto floating_sine = radians | std::views::transform([](auto value) {
//                              return std::sin(value);
//                          });
//
//     std::ranges::for_each(floating_sine, [](std::floating_point auto value) {
//         std::println("floating_sine: {}", value);
//     });
//
//     auto radians_as_qs = radians | std::views::transform([](auto value) {
//                              return bit::qs<3, 1>(value);
//                          });
//
//     std::ranges::for_each(radians_as_qs, [](bit::qformatted auto value) {
//         std::println("radians_as_qs: {}", value.template as<float>());
//     });
//
//     auto fixed_sine = radians_as_qs | std::views::transform([](auto value) {
//                           return bit::saturated_sine<bit::qs<0, 4>>(value);
//                       });
//
//     std::ranges::for_each(std::views::zip(fixed_sine, radians_as_qs), [](auto
//     zipped) {
//         auto [sine, radians] = zipped;
//         std::println("fixed_sine: {} ({})", sine.template as<float>(),
//         radians.template as<float>());
//     });
//     //
//     // // auto r = bit::sine<bit::qu<0, 7>>(bit::qu<0, 8>{6.28});
//     // REQUIRE(true);
// }

TEST_CASE("Compute sine")
{
    auto radians = bit::qu<3, 10>{std::numbers::pi_v<float> / 2.f};
    auto v       = bit::cordic::sine<bit::qs<3, 20>>(radians);
    CHECK(true);
}
