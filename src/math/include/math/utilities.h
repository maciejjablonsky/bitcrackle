#pragma once

#include <concepts>
#include <ranges>
#include <utility>

namespace bit
{
template <typename T> [[nodiscard]] constexpr T as(auto value) noexcept
{
    return static_cast<T>(value);
}

constexpr auto loop_i(size_t N) noexcept
{
    return std::views::iota(0u) | std::views::take(N);
}

template <typename ValueT, typename LowT, typename HighT>
constexpr ValueT clamp(const ValueT& value,
                       const LowT& low,
                       const HighT& high) noexcept
{
    if (std::cmp_less(value, low))
    {
        return static_cast<ValueT>(low);
    }
    if (std::cmp_greater(value, high))
    {
        return static_cast<ValueT>(high);
    }
    return value;
}

template <typename It, typename T>
    requires std::
        convertible_to<T, std::remove_cvref_t<decltype(*std::declval<It>())>>
    void linear_space(It begin, It end, T low, T high) noexcept
{
    using value_type = decltype(*begin);
    auto count       = end - begin;
    if (count <= 1)
    {
        return;
    }

    float increment = (high - low) / count;

    for (decltype(count) i = 0; i < count; ++i)
    {
        *(begin + i) = low + i * increment;
    }
}
} // namespace bit
