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

// constexpr auto loop_i(size_t N) noexcept
// {
//     return std::views::iota(0u) | std::views::take(N);
// }

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
} // namespace bit
