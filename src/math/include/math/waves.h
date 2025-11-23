#pragma once

#include "math/qnumber.h"
#include "math/utilities.h"

#include <array>
#include <bit>
#include <concepts>
#include <numbers>
#include <ranges>
#include <stdexcept>
#include <type_traits>

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
    auto result          = first.template as<widest_qnumber>() -
                  second.template as<widest_qnumber>() +
                  third.template as<widest_qnumber>();
    // - fourth.as<widest_qnumber>();

    return result;
    // return arg_to_9;
}

namespace cordic
{
template <size_t N> constexpr auto compute_K() noexcept -> double
{
    double denominator = 1.0;
    for (auto k : loop_i(N))
    {
        auto power = gcem::pow(2.0, -2.0 * k);
        denominator *= (1.0 + power);
    }
    return 1.0 / gcem::sqrt(denominator);
}

template <qformatted T, size_t N> constexpr auto compute_theta() noexcept
{
    auto arctan = gcem::atan(1.0 / (1 << N));
    return T{arctan};
}

template <size_t IterationIndex,
          size_t IterationSteps,
          qformatted XT,
          qformatted YT,
          qformatted AngleT>
auto sine_bounded_iteration_step(XT x, YT y, AngleT angle) noexcept
{
    constexpr auto theta = compute_theta<AngleT, IterationIndex>();
    auto x_modifier      = YT{as_is_t{
        static_cast<typename YT::value_type>(y.raw() >> IterationIndex)}};
    auto y_modifier      = XT{as_is_t{
        static_cast<typename XT::value_type>(x.raw() >> IterationIndex)}};
    if (angle.is_positive())
    {
        x     = x.saturate_subtract(x_modifier);
        y     = y.saturate_add(y_modifier);
        angle = angle.saturate_subtract(theta);
    }
    else
    {
        x     = x.saturate_add(x_modifier);
        y     = y.saturate_subtract(y_modifier);
        angle = angle.saturate_add(theta);
    }

    if constexpr (IterationIndex < IterationSteps)
    {
        return sine_bounded_iteration_step<IterationIndex + 1, IterationSteps>(
            x, y, angle);
    }
    else
    {
        return y;
    }
}

template <size_t MaxBits, qformatted ArgT>
    requires(MaxBits > 0u)
constexpr auto sine_bounded(ArgT radians) noexcept
{
    constexpr ArgT minus_half_pi{-90 * std::numbers::pi / 180};
    constexpr ArgT plus_half_pi{90 * std::numbers::pi / 180};
    assert(minus_half_pi <= radians && radians <= plus_half_pi);

    constexpr size_t iterations = MaxBits;
    constexpr qs<MaxBits / 2, MaxBits / 2 + MaxBits % 2> K{
        compute_K<iterations>()};

    auto angle = [radians] {
        if constexpr (bit::safely_convertible<
                          decltype(radians),
                          qs<MaxBits / 2, MaxBits / 2 + MaxBits % 2>>)
        {
            return radians
                .template as<qs<MaxBits / 2, MaxBits / 2 + MaxBits % 2>>();
        }
        else
            return radians.template narrow_as<
                qs<MaxBits / 2, MaxBits / 2 + MaxBits % 2>>();
    }();
    constexpr qs<MaxBits / 2, MaxBits / 2 + MaxBits % 2> one{1.f};
    auto x    = K;
    auto y    = qs<MaxBits / 2, MaxBits / 2 + MaxBits % 2>{0.f};
    auto sine = sine_bounded_iteration_step<0, iterations>(x, y, angle);
    auto sine_reduced =
        sine.template narrow_as<qs<1, decltype(sine)::fraction_bits>>();
    // return sine_reduced.template as<qs<1, MaxBits - 1>>();
    return sine_reduced;
}

template <size_t MaxBits, qformatted ArgT>
    requires(MaxBits > 0u)
constexpr auto sine(ArgT radians) noexcept
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

    return sine_bounded<MaxBits>(radians);
}
} // namespace cordic

template <typename FuncT> class oscilator
{
  public:
    using value_type = std::invoke_result_t<FuncT, uint64_t>;

  private:
    FuncT _generator{};
    const float _wave_frequency{};
    const uint32_t _sampling_frequency{};
    int64_t _sample_index{};

  public:
    oscilator(FuncT generator,
              float wave_frequency,
              uint32_t sampling_frequency)
        : _generator(generator), _wave_frequency(wave_frequency),
          _sampling_frequency(sampling_frequency),
          _sample_index(-_wave_frequency)
    {
        assert(0 != _wave_frequency);
        assert(0 != _sampling_frequency);
    }

    value_type next() noexcept
    {
        _sample_index = std::round(_sample_index + _wave_frequency);
        _sample_index %= _sampling_frequency;

        auto phase = 2 * std::numbers::pi *
                     (static_cast<double>(_sample_index) / _sampling_frequency);
        return _generator(phase);
    }

    template <std::ranges::forward_range RangeT>
    void next(RangeT&& range) noexcept
    {
        std::ranges::for_each(range, [this](auto& value) { value = next(); });
    }
};

} // namespace bit
