#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <execution>
#include <expected>
#include <ranges>
#include <stdexcept>

namespace bit
{
template <typename T> struct pearson_correlation_partial_result
{
    T sum_of_x{};
    T sum_of_y{};
    T sum_of_x_squared{};
    T sum_of_y_squared{};
    T sum_of_x_times_y{};
    T n{};

    T apply_formula() const
    {
        auto nominator = n * sum_of_x_times_y - sum_of_x * sum_of_y;
        auto denominator =
            std::sqrt(n * sum_of_x_squared - sum_of_x * sum_of_x) *
            std::sqrt(n * sum_of_y_squared - sum_of_y * sum_of_y);
        return nominator / denominator;
    }
};

template <std::ranges::input_range LeftRangeT,
          std::ranges::input_range RightRangeT>
    requires std::floating_point<std::ranges::range_value_t<LeftRangeT>> and
             std::floating_point<std::ranges::range_value_t<RightRangeT>>
[[nodiscard]] constexpr auto pearson_correlation(const LeftRangeT& left,
                                                 const RightRangeT& right)
    -> std::expected<
        std::common_type_t<std::ranges::range_value_t<LeftRangeT>,
                           std::ranges::range_value_t<RightRangeT>>,
        std::exception>

{
    using T = std::common_type_t<std::ranges::range_value_t<LeftRangeT>,
                                 std::ranges::range_value_t<RightRangeT>>;

    if (left.empty())
    {
        return std::unexpected(std::invalid_argument{"left range is empty"});
    }

    if (left.size() != right.size())
    {
        return std::unexpected(std::invalid_argument{
            "left and right ranges have different lengths"});
    }

    using partial_result_t = pearson_correlation_partial_result<T>;

    auto partial_results = std::transform_reduce(
#if not(defined(__clang__) and defined(__apple_build_version__))
        std::execution::unseq,
#endif
        left.begin(),
        left.end(),
        right.begin(),
        partial_result_t{},
        [](const partial_result_t& a, const partial_result_t& b) {
            return partial_result_t{a.sum_of_x + b.sum_of_x,
                                    a.sum_of_y + b.sum_of_y,
                                    a.sum_of_x_squared + b.sum_of_x_squared,
                                    a.sum_of_y_squared + b.sum_of_y_squared,
                                    a.sum_of_x_times_y + b.sum_of_x_times_y,
                                    a.n + b.n};
        },
        [](std::floating_point auto x, std::floating_point auto y) {
            return partial_result_t{x, y, x * x, y * y, x * y, 1};
        });

    return {partial_results.apply_formula()};
}

} // namespace bit
