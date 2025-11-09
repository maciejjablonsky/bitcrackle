#pragma once

#include "math/qnumber.h"
#include "math/utilities.h"
#include <array>
#include <bit>
#include <numbers>
#include <ranges>
#include <stdexcept>

#pragma warning(push)
#pragma warning(disable : 5045)
#include <gcem.hpp>
#pragma warning(pop)

namespace bit
{
constexpr std::unsigned_integral auto factorial(
    std::unsigned_integral auto N) noexcept
{
    using T = decltype(N);
    auto y  = as<T>(1);
    for (auto i = as<T>(1); i <= N; ++i)
    {
        y *= i;
    }
    return y;
}

template <std::unsigned_integral auto N>
consteval std::unsigned_integral auto factorial() noexcept
{
    return factorial(N);
}

static_assert(factorial(0u) == 1);
static_assert(factorial<0u>() == 1);
static_assert(factorial(1u) == 1);
static_assert(factorial<1u>() == 1);
static_assert(factorial(2u) == 2);
static_assert(factorial<2u>() == 2);
static_assert(factorial(3u) == 6);
static_assert(factorial<3u>() == 6);
static_assert(factorial(4u) == 24);
static_assert(factorial<4u>() == 24);

// TODO: find out constexpr version
[[nodiscard]] constexpr size_t factorial_bit_width(size_t N)
{
    switch (N)
    {
    case 0:
        return 1;
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    case 4:
        return 5;
    case 5:
        return 7;
    case 6:
        return 10;
    case 7:
        return 13;
    case 8:
        return 16;
    case 9:
        return 19;
    case 10:
        return 22;
    case 11:
        return 26;
    case 12:
        return 29;
    case 13:
        return 33;
    case 14:
        return 37;
    case 15:
        return 41;
    case 16:

        return 45;
    case 17:
        return 49;
    case 18:
        return 53;
    case 19:
        return 57;
    case 20:
        return 62;
    case 21:
        return 66;
    default:
        throw std::range_error{
            "factorial_bit_width() is not implemented for N > 21"};
    }

    // if constexpr (N )
    // auto sum = 0.L;
    // for (auto i = 1; i <= N; ++i)
    // {
    //     sum += std::log2(i);
    // }
    // return std::lround(sum);
}

static_assert(factorial_bit_width(0) == 1);
static_assert(factorial_bit_width(1) == 1);
static_assert(factorial_bit_width(2) == 2);
static_assert(factorial_bit_width(3) == 3);
static_assert(factorial_bit_width(4) == 5);
static_assert(factorial_bit_width(5) == 7);
static_assert(factorial_bit_width(6) == 10);

// constexpr qformatted auto factorial(qformatted_integral auto N) noexcept
// {
//
//     // constexpr auto raw = N.raw();
//     // constexpr size_t width = factorial_bit_width<raw>();
//     // auto result            = factorial(raw);
//     // return qu<width, 0>(result);
//     return qu<3, 0>(0u);
// }

// static_assert(qu<2, 0>(3u) == qu<2, 0>(3u));
// static_assert(factorial(qu<2, 0>{3u}) == qu<3, 0>{0u});

template <size_t Power, qformatted T> auto power_with_integral_exponent(T x)
{
    // auto result = x;
    // for (auto i = 0; i < Power - 1; ++i)
    // {
    //    result = result. * x;
    // }
    // return result;
}

template <size_t N> constexpr qformatted_integral auto qnumber_factorial()
{
    using type = qu<factorial_bit_width(N), 0>;
    return type{factorial<N>()};
}

float float_sine(float radians)
{
    float first  = radians;
    float second = (radians * radians * radians) / 6.f;
    float third  = (radians * radians * radians * radians * radians) / 120.f;
    float fourth =
        (radians * radians * radians * radians * radians * radians * radians) /
        5040.f;
    auto result = first - second + third - fourth;
    return result;
}

template <qformatted ArgT> constexpr auto saturated_sine(ArgT radians) noexcept
{

    auto first = radians;

    auto arg_to_3 =
        radians.as_signed().accurate_mul(radians).accurate_mul(radians);
    constexpr auto factorial_3 = qnumber_factorial<3u>();
    auto second                = arg_to_3.accurate_div(factorial_3);

    auto arg_to_5 = arg_to_3.accurate_mul(radians).accurate_mul(radians);
    constexpr auto factorial_5 = qnumber_factorial<5u>();
    auto third                 = arg_to_5.accurate_div(factorial_5);

    // auto arg_to_7 = arg_to_5.accurate_mul(radians).accurate_mul(radians);
    // constexpr auto factorial_7 = qnumber_factorial<7u>();
    // auto fourth = arg_to_7.accurate_div(factorial_7);
    //
    // auto arg_to_9 = arg_to_7.accurate_mul(radians).accurate_mul(radians);
    // constexpr auto factorial_9 = qnumber_factorial<9u>();
    // auto fifth = arg_to_9.accurate_div(factorial_9);

    using widest_qnumber = std::remove_cvref_t<decltype(third)>;
    auto result = first.as<widest_qnumber>() - second.as<widest_qnumber>() +
                  third.as<widest_qnumber>();
    // - fourth.as<widest_qnumber>();

    return result;
    // return arg_to_9;
}

namespace cordic
{
template <size_t N> constexpr auto compute_K() noexcept -> double
{
    double K = 1.0;
    for (auto i : loop_i(N))
    {
        auto power       = gcem::pow(2.0, -2.0 * i);
        auto sqrt        = gcem::sqrt(1.0 + power);
        auto coefficient = 1.0 / sqrt;
        K *= coefficient;
    }
    return K;
}

template <qformatted T, size_t N>
constexpr auto compute_theta_table() noexcept -> std::array<T, N>
{
    std::array<T, N> theta_table;
    for (auto i : loop_i(N))
    {
        theta_table[i] = gcem::atan2(1.0, gcem::pow(2.0, i));
    }
    return theta_table;
}

template <qformatted WidestT, qformatted ArgT>
constexpr WidestT sine_bounded(ArgT radians) noexcept
{
    constexpr ArgT minus_half_pi{-90 * std::numbers::pi / 180};
    constexpr ArgT plus_half_pi{90 * std::numbers::pi / 180};
    assert(minus_half_pi <= radians && radians <= plus_half_pi);

    constexpr size_t iterations = 16;
    constexpr qu<0, WidestT::bits> K{compute_K<iterations>()};
    constexpr auto theta_table = compute_theta_table<WidestT, iterations>();

    WidestT angle = radians.template as<WidestT>();
    WidestT theta{};
    WidestT x{1.0};
    WidestT y{0.0};
    constexpr qs<1, 0> one{1.0};
    qu<1, WidestT::bits - 1> P2i{1.0};
    for (qformatted auto arc_tanget : theta_table)
    {
        const auto sigma = theta < angle ? one : -one;
        const auto sigma_times_arc_tangent =
            sigma.saturate_multiply<WidestT>(arc_tanget);
        theta = theta.saturate_add(sigma_times_arc_tangent);
        const auto sigma_times_P2i = sigma.saturate_multiply<WidestT>(P2i);
        x                          = x.saturate_subtract<WidestT>(
            y.saturate_multiply<WidestT>(sigma_times_P2i));
        y = y.saturate_add<WidestT>(
            x.saturate_multiply<WidestT>(sigma_times_P2i));
        P2i = as_is_t{P2i.raw() >> 1};
    }
    return y.saturate_multiply<WidestT>(K);
}

template <qformatted WidestT, qformatted ArgT>
constexpr WidestT sine(ArgT radians) noexcept
{
    constexpr ArgT minus_pi{-std::numbers::pi};
    constexpr ArgT minus_two_pi{-2 * std::numbers::pi};
    constexpr ArgT plus_pi{std::numbers::pi};
    constexpr ArgT plus_two_pi{2 * std::numbers::pi};
    constexpr ArgT minus_half_pi{-std::numbers::pi / 2};
    constexpr ArgT plus_half_pi{std::numbers::pi / 2};

    while (radians < minus_pi or plus_pi < radians)
    {
        if (radians > plus_pi)
        {
            radians = radians.saturate_subtract(plus_two_pi);
        }
        if (radians < minus_pi)
        {
            radians = radians.saturate_add(plus_two_pi);
        }
    }

    if (radians > plus_half_pi)
    {
        radians = plus_pi.saturate_subtract(radians);
    }
    else if (radians < minus_half_pi)
    {
        radians = minus_pi.saturate_subtract(radians);
    }

    return sine_bounded<WidestT>(radians);
}
} // namespace cordic

// template<qformatted ReturnT> [[nodiscard]] constexpr ReturnT
// saturated_sine(qformatted auto radians) noexcept
// {
//     auto first = radians;
//
//     auto arg_to_3 = radians.accurate_mul(radians).accurate_mul(radians);
//     constexpr auto factorial_3 = qnumber_factorial<3u>();
//     auto second =  arg_to_3.accurate_div(factorial_3);
//
//
//     auto result = first.template narrow_as<ReturnT>() - second.template
//     narrow_as<ReturnT>();;
//     // return result.template narrow_as<ReturnT>();
//     return result;
// }

// constexpr auto sine_of_pi = accurate_sine(qu<2, 3>{3.14159265359});
} // namespace bit
