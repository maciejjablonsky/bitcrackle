#include <catch2/catch_test_macros.hpp>
#include <math/qnumber.h>
#include <print>

template <typename T> using nl = std::numeric_limits<T>;

size_t fraction_bits_count(float fractional_part)
{
    size_t result = 0;
    while (fractional_part != 0.f)
    {
        fractional_part *= 2.f;
        ++result;
    }
    return result;
}

TEST_CASE("brute force compute bits boundaries", "[boundsBruteForce]")
{
    constexpr size_t dividend_integer_bits  = 1;
    constexpr size_t dividend_fraction_bits = 5;
    constexpr size_t divisor_integer_bits   = 3;
    constexpr size_t divisor_fraction_bits  = 8;
    using dividend_type =
        bit::qu<dividend_integer_bits, dividend_fraction_bits>;
    using divisor_type = bit::qu<divisor_integer_bits, divisor_fraction_bits>;

    size_t result_max_integer_bits  = 0;
    size_t result_max_fraction_bits = 0;

    for (size_t dividend_integer_bits_i = 0;
         dividend_integer_bits_i < (1 << dividend_integer_bits);
         ++dividend_integer_bits_i)
    {
        for (size_t dividend_fraction_bits_i = 0;
             dividend_fraction_bits_i < 1 << dividend_fraction_bits;
             ++dividend_fraction_bits_i)
        {
            dividend_type::value_type dividend_raw = 0;
            bit::assign_bits<dividend_type::msb, dividend_type::fraction_bits>(
                dividend_raw, dividend_integer_bits_i);
            bit::assign_bits<dividend_type::fraction_bits - 1, 0>(
                dividend_raw, dividend_fraction_bits_i);
            dividend_type dividend = bit::as_is_t{dividend_raw};

            for (size_t divisor_integer_bits_i = 0;
                 divisor_integer_bits_i < 1 << divisor_integer_bits;
                 ++divisor_integer_bits_i)
            {
                for (size_t divisor_fraction_bits_i = 0;
                     divisor_fraction_bits_i < 1 << divisor_fraction_bits;
                     ++divisor_fraction_bits_i)
                {
                    divisor_type::value_type divisor_raw = 0;
                    bit::assign_bits<divisor_type::msb,
                                     divisor_type::fraction_bits>(
                        divisor_raw, divisor_integer_bits_i);
                    bit::assign_bits<divisor_type::fraction_bits - 1, 0>(
                        divisor_raw, divisor_fraction_bits_i);
                    divisor_type divisor = bit::as_is_t{divisor_raw};
                    if (dividend.raw() and divisor.raw())
                    {
                        auto dividend_raw_shifted =
                            dividend.raw()
                            << (divisor.integer_bits + divisor.fraction_bits);
                        auto result = dividend_raw_shifted / divisor.raw();
                        auto dividend_floating = dividend.as<float>();
                        auto divisor_floating  = divisor.as<float>();

                        auto accurate_div_result =
                            dividend.accurate_divide(divisor);
                        float result_floating =
                            dividend_floating / divisor_floating;
                        float result_floating_integer_part{};
                        float result_floating_fraction_part = std::modf(
                            result_floating,
                            std::addressof(result_floating_integer_part));

                        size_t result_integer_bits = static_cast<size_t>(0);
                        if (std::abs(result_floating) >= 1.f)
                        {
                            result_integer_bits = static_cast<size_t>(std::log2(
                                                      result_floating)) +
                                                  1;
                        }

                        size_t result_fraction_bits = static_cast<size_t>(0);
                        if (result_floating_fraction_part != 0.f)
                        {
                            result_fraction_bits =
                                static_cast<size_t>(std::abs(std::round(
                                    std::log2(result_floating_fraction_part))));
                        }

                        [[maybe_unused]] auto result_all_bits =
                            static_cast<size_t>(std::log2(result)) + 1;
                        // auto result_fraction_bits = result_all_bits -
                        // result_integer_bits;
                        bool is_new_max_integer_bits =
                            result_integer_bits > result_max_integer_bits;
                        bool is_new_max_fraction_bits =
                            result_fraction_bits > result_max_fraction_bits;
                        if (is_new_max_integer_bits)
                        {
                            result_max_integer_bits = result_integer_bits;
                        }
                        if (is_new_max_fraction_bits)
                        {
                            result_max_fraction_bits = result_fraction_bits;
                        }
                        if (is_new_max_integer_bits or is_new_max_fraction_bits)
                        {
                            std::print(
                                "{:03x}({:05f})[{:02b},{:02b}]<{}, {}> / "
                                "{:03x}({:05f})[{:03b},{:03b}]<{}, {}>",
                                dividend.raw(),
                                dividend.as<double>(),
                                dividend.integer_part(),
                                dividend.fraction_part(),
                                dividend_type::integer_bits,
                                dividend_type::fraction_bits,
                                divisor.raw(),
                                divisor.as<double>(),
                                divisor.integer_part(),
                                divisor.fraction_part(),
                                divisor_type::integer_bits,
                                divisor_type::fraction_bits);
                            std::println(
                                " = {:010b}({:05f} ref | {:05f} computed)<{}?, "
                                "{}?> ",
                                result,
                                dividend.as<float>() / divisor.as<float>(),
                                accurate_div_result.as<float>(),
                                accurate_div_result.integer_bits,
                                accurate_div_result.fraction_bits);
                        }
                    }
                }
            }
        }
    }
    std::println(
        "for dividend<{},{}> and divisor<{},{}>, result_max_format = <{}, {}>",
        dividend_type::integer_bits,
        dividend_type::fraction_bits,
        divisor_type::integer_bits,
        divisor_type::fraction_bits,
        result_max_integer_bits,
        result_max_fraction_bits);
}

TEST_CASE("accurate: max() unsigned qformat / max() same unsigned qformat")
{
    constexpr auto a  = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b  = std::numeric_limits<bit::qu<3, 2>>::max();
    auto result       = a.accurate_divide(b);
    auto float_result = a.as<float>() / b.as<float>();
    CHECK(result.is_nearest_to(float_result));
}

TEST_CASE("saturate: max() unsigned qformat / max() same unsigned qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = std::numeric_limits<bit::qu<3, 2>>::max();
    auto result      = a.saturate_divide<bit::qu<3, 1>>(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("accurate: max() unsigned qformat / -1 * max() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<3, 2>>::max();
    auto result      = a.accurate_divide(b);
    auto is_nearest  = result.is_nearest_to(a.as<float>() / b.as<float>());
    CHECK(is_nearest);
}

TEST_CASE("saturate: max() unsigned qformat / -1 * max() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<3, 2>>::max();
    auto result      = a.saturate_divide<bit::qs<3, 1>>(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("accurate: max() unsigned qformat / -1 * lowest() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<2, 4>>::lowest();
    auto result      = a.accurate_divide(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("accurate: max() unsigned qformat / -1 * min() signed qformat")
{
    constexpr auto a = std::numeric_limits<bit::qu<3, 2>>::max();
    constexpr auto b = -std::numeric_limits<bit::qs<2, 4>>::min();
    auto result      = a.accurate_divide(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("accurate: lowest() signed qformat / max() unsigned qformat")
{
    constexpr auto a = std::numeric_limits<bit::qs<2, 4>>::lowest();
    constexpr auto b = std::numeric_limits<bit::qu<9, 2>>::max();
    auto result      = a.accurate_divide(b);
    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}

TEST_CASE("1/3")
{
    constexpr bit::qs<1, 5> a{1.f};
    constexpr bit::qs<2, 0> b{3.f};
    auto result     = a.accurate_divide(b);
    auto sat_result = a.saturate_divide<bit::qs<1, 2>>(b);

    CHECK(result.is_nearest_to(a.as<float>() / b.as<float>()));
}
