#pragma once

#include <concepts>

namespace bit::tl
{
template <std::floating_point T> constexpr unsigned long long ullround(T x)
{
#ifndef NDEBUG
    constexpr auto max = std::numeric_limits<unsigned long long>::max();
    constexpr auto min = std::numeric_limits<unsigned long long>::min();
    constexpr auto max_and_half = static_cast<T>(max) + static_cast<T>(0.5);
    constexpr auto min_and_half = static_cast<T>(min) - static_cast<T>(0.5);
    if (x >= max_and_half || x <= min_and_half)
    {
        throw std::domain_error(
            "ullround: value out of range for unsigned long long");
    }
#endif

    auto result = static_cast<unsigned long long>((x >= 0) ? x + 0.5 : x - 0.5);
    return result;
}

template <std::floating_point T> constexpr long long llround(T x)
{
#ifndef NDEBUG
    constexpr auto max          = std::numeric_limits<long long>::max();
    constexpr auto min          = std::numeric_limits<long long>::min();
    constexpr auto max_and_half = static_cast<T>(max) + static_cast<T>(0.5);
    constexpr auto min_and_half = static_cast<T>(min) - static_cast<T>(0.5);
    if (x >= max_and_half || x <= min_and_half)
    {
        throw std::domain_error("lround: value out of range for long long");
    }
#endif

    [[maybe_unused]] auto x_as_integer = static_cast<long long>(x);
    auto result = static_cast<long long>((x >= 0) ? x + 0.5 : x - 0.5);
    return result;
}

template <bool IsSigned, std::floating_point T> constexpr auto round(T x)
{
    if constexpr (IsSigned)
    {
        return tl::llround(x);
    }
    else
    {
        return tl::ullround(x);
    }
}
} // namespace bit::tl
