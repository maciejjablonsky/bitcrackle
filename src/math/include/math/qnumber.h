#pragma once

#include "math/bits.h"
#include "math/qnumber/round.h"
#include "math/utilities.h"

#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <concepts>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bit
{
template <size_t IntegerBits, size_t FractionBits>
concept signed_container_exists = IntegerBits + FractionBits <= 63;
template <size_t IntegerBits, size_t FractionBits>
concept unsigned_container_exists = IntegerBits + FractionBits <= 64;

template <size_t IntegerBits, size_t FractionBits>
    requires signed_container_exists<IntegerBits, FractionBits>
consteval auto compute_minimal_signed_type()
{
    constexpr size_t total_bits = IntegerBits + FractionBits;

    if constexpr (width<int8_t>() >= total_bits)
        return int8_t{};
    else if constexpr (width<int16_t>() >= total_bits)
        return int16_t{};
    else if constexpr (width<int32_t>() >= total_bits)
        return int32_t{};
    else if constexpr (width<int64_t>() >= total_bits)
        return int64_t{};
    else
    {
        std::unreachable();
    }
    // else
    //     static_assert(total_bits <= 63,
    //                   "No suitable signed integer type available.");
}

template <size_t IntegerBits, size_t FractionBits>
consteval auto compute_minimal_unsigned_type()
    requires unsigned_container_exists<IntegerBits, FractionBits>
{
    constexpr size_t total_bits = IntegerBits + FractionBits;

    if constexpr (width<uint8_t>() >= total_bits)
        return uint8_t{};
    else if constexpr (width<uint16_t>() >= total_bits)
        return uint16_t{};
    else if constexpr (width<uint32_t>() >= total_bits)
        return uint32_t{};
    else if constexpr (width<uint64_t>() >= total_bits)
        return uint64_t{};
    else
    {
        std::unreachable();
    }
    // static_assert(total_bits <= 64,
    //               "No suitable signed integer type available.");
}

template <size_t IntegerBits, size_t FractionBits, typename ContainerT>
concept wide_enough = std::integral<ContainerT> and
                      (IntegerBits + FractionBits) <= width<ContainerT>();

template <size_t IntegerBits, size_t FractionBits>
    requires signed_container_exists<IntegerBits, FractionBits>
using minimal_signed_type =
    decltype(compute_minimal_signed_type<IntegerBits, FractionBits>());

template <size_t IntegerBits, size_t FractionBits>
    requires unsigned_container_exists<IntegerBits, FractionBits>
using minimal_unsigned_type =
    decltype(compute_minimal_unsigned_type<IntegerBits, FractionBits>());

static_assert(std::same_as<minimal_signed_type<0, 0>, int8_t>);
static_assert(std::same_as<minimal_signed_type<0, 7>, int8_t>);
static_assert(std::same_as<minimal_signed_type<1, 7>, int16_t>);
static_assert(std::same_as<minimal_signed_type<1, 8>, int16_t>);
static_assert(std::same_as<minimal_signed_type<63, 0>, int64_t>);

static_assert(std::same_as<minimal_unsigned_type<0, 0>, uint8_t>);
static_assert(std::same_as<minimal_unsigned_type<1, 7>, uint8_t>);
static_assert(std::same_as<minimal_unsigned_type<1, 8>, uint16_t>);
static_assert(std::same_as<minimal_unsigned_type<8, 8>, uint16_t>);
static_assert(std::same_as<minimal_unsigned_type<1, 32>, uint64_t>);
static_assert(std::same_as<minimal_unsigned_type<32, 32>, uint64_t>);

template <typename LhsQ, typename RhsQ>
concept common_denominator = (LhsQ::fraction_bits == RhsQ::fraction_bits);

template <typename T>
concept qformatted = requires(const T& t) {
    { T::integer_bits } -> std::convertible_to<size_t>;
    { T::fraction_bits } -> std::convertible_to<size_t>;
};

template <typename T>
concept qformatted_integral = qformatted<T> and T::fraction_bits == 0;

template <typename T, typename U>
concept wider_integer =
    qformatted<T> and qformatted<U> and (T::integer_bits < U::integer_bits);
template <typename T, typename U>
concept wider_fraction =
    qformatted<T> and qformatted<U> and (T::fraction_bits < U::fraction_bits);
template <typename T, typename U>
concept narrower_integer =
    qformatted<T> and qformatted<U> and (T::integer_bits > U::integer_bits);
template <typename T, typename U>
concept narrower_fraction =
    qformatted<T> and qformatted<U> and (T::fraction_bits > U::fraction_bits);

template <typename FromT, typename ToT>
concept safely_convertible =
    qformatted<FromT> and qformatted<ToT> and
    (not narrower_integer<FromT, ToT> and not narrower_fraction<FromT, ToT>);

template <std::integral T = int> struct as_is_t final
{
    T value{};
    constexpr explicit as_is_t() = default;
    constexpr explicit as_is_t(T value) : value{value}
    {
    }
};
constexpr as_is_t as_is{};

template <size_t IntegerBits,
          size_t FractionBits,
          bool IsSigned,
          typename = void>
struct value_type_for
{
};

template <size_t IntegerBits, size_t FractionBits, bool IsSigned>
    requires IsSigned
struct value_type_for<IntegerBits, FractionBits, IsSigned>
{
    using type = minimal_signed_type<IntegerBits, FractionBits>;
};

template <size_t IntegerBits, size_t FractionBits, bool IsSigned>
    requires !IsSigned
             struct value_type_for<IntegerBits, FractionBits, IsSigned>
{
    using type = minimal_unsigned_type<IntegerBits, FractionBits>;
};

constexpr std::array powers_of_two = [] {
    std::array<long double, 65> power{};
    power[0] = 1;
    for (size_t i = 1; i < power.size(); ++i)
    {
        power[i] = power[i - 1] * 2;
    }
    return power;
}();

template <size_t IntegerBits, size_t FractionBits, std::integral T>
    requires wide_enough<IntegerBits, FractionBits, T>
class qnumber
{
  private:
    T value_ = 0;

    template <std::floating_point FloatingT>
    static constexpr T floating_to_fixed_(FloatingT value)
    {
        if (value == 0.f)
        {
            return 0;
        }
        if constexpr (not is_signed)
        {
            if (value < 0.f)
            {
                throw std::range_error{
                    "floating point value is negative but T isn't signed"};
            }
        }

        if (not std::is_constant_evaluated())
        {
            auto bits_needed_for_integer = std::floor(std::log2(value) + 1);
            if (bits_needed_for_integer > 0 and
                bits_needed_for_integer > integer_bits)
            {
                throw std::range_error{
                    "floating point value doesn't fit on integer bits"};
            }
        }
        constexpr auto power_of_two = powers_of_two[FractionBits];
        auto fixed_as_floating      = value * power_of_two;
        auto fixed =
            static_cast<T>(bit::tl::round<is_signed>(fixed_as_floating));

        return fixed;
    }

    template <std::floating_point ToT>
    static constexpr auto fixed_to_floating_(T value)
    {
        constexpr auto power_of_two = powers_of_two[FractionBits];
        return bit::as<ToT>(bit::as<ToT>(value) / power_of_two);
    }

  public:
    auto static constexpr integer_bits  = IntegerBits;
    auto static constexpr fraction_bits = FractionBits;
    auto static constexpr bits          = integer_bits + fraction_bits;
    auto static constexpr msb           = integer_bits + fraction_bits - 1;
    auto static constexpr lsb           = 0ull;

    using value_type                = T;
    auto static constexpr is_signed = std::signed_integral<value_type>;

    constexpr qnumber() = default;

    constexpr explicit qnumber(std::floating_point auto value)
        requires is_signed
    {
        value_ = floating_to_fixed_(value);
    }

    constexpr qnumber& operator=(std::floating_point auto value)
        requires is_signed
    {
        value_ = floating_to_fixed_(value);
        return *this;
    }

    constexpr explicit qnumber(std::floating_point auto value)
        requires not is_signed
    {
        assert(value >= 0);
        value_ = floating_to_fixed_(value);
    }

    constexpr qnumber& operator=(std::floating_point auto value)
        requires not is_signed
    {
        assert(value >= 0);
        value_ = floating_to_fixed_(value);
        return *this;
    }

    constexpr explicit qnumber(std::unsigned_integral auto value)
        requires not is_signed
    {
        assert(bit::as<size_t>(std::bit_width(value)) <= integer_bits);
        assign_bits<msb, fraction_bits>(value_, value);
    }

    template <std::integral U>
        requires(is_signed == std::is_signed_v<U>)
    constexpr qnumber(as_is_t<U> as_is_v) noexcept : value_{as_is_v.value}
    {
    }

    template <std::integral U>
        requires(is_signed == std::is_signed_v<U>)
    constexpr qnumber& operator=(as_is_t<U> as_is_v) noexcept
    {
        value_ = static_cast<T>(as_is_v.value);
        return *this;
    }

    constexpr auto operator<=>(const qnumber&) const = default;

    // TODO: probably kind of copy_sign_bit() between different signed_integral
    // types is needed and copying numeric bits separately constexpr explicit
    // qnumber(std::signed_integral auto value) requires is_signed :
    // value_{value}
    // {
    //     assert(std::bit_width(value) <= integer_bits);
    //     assign_bits<msb, fraction_bits>(value_, value);
    // }

    template <std::floating_point ToT>
    [[nodiscard]] constexpr ToT as() const noexcept
    {
        return fixed_to_floating_<ToT>(value_);
    }

    template <qformatted ToT>
        requires safely_convertible<qnumber, ToT>
    [[nodiscard]] constexpr ToT as() const noexcept
    {
        if constexpr (is_signed && not ToT::is_signed)
        {
            if (value_ < 0)
            {
                throw std::range_error{
                    "can't cast negative value into unsigned type"};
            }
        }
        auto new_raw = [this] {
            constexpr size_t shift = ToT::fraction_bits - fraction_bits;
            if constexpr (shift > 0)
            {
                return static_cast<typename ToT::value_type>(raw() << shift);
            }
            else if constexpr (shift == 0)
            {
                return static_cast<typename ToT::value_type>(raw());
            }
        }();
        ToT result{as_is_t{new_raw}};
        return result;
    }

    [[nodiscard]] constexpr qformatted auto as_signed() const noexcept
    {
        return as<qnumber<integer_bits,
                          fraction_bits,
                          minimal_signed_type<integer_bits, fraction_bits>>>();
    }

    template <qformatted ToT> constexpr ToT narrow_as() const noexcept
    {
        static_assert(
            not safely_convertible<qnumber, ToT>,
            "requested result type is not narrower — use 'as' instead");

        using wide_value_type = value_type;

        constexpr auto raw_return_lowest =
            std::numeric_limits<ToT>::lowest().raw();
        constexpr auto raw_return_max = std::numeric_limits<ToT>::max().raw();

        constexpr size_t shift = fraction_bits - ToT::fraction_bits;

        std::integral auto wide_value = raw();
        wide_value >>= shift;

        std::integral auto saturated =
            bit::clamp(wide_value, raw_return_lowest, raw_return_max);

        return {as_is_t{static_cast<typename ToT::value_type>(saturated)}};
    }

    [[nodiscard]] constexpr T raw() const noexcept
    {
        return value_;
    }

    template <qformatted ArgT>
        requires common_denominator<qnumber, ArgT>
    [[nodiscard]] constexpr auto add(ArgT argument) const noexcept
    {
        constexpr size_t new_integer_bits =
            std::max(ArgT::integer_bits, integer_bits) + 1;
        constexpr size_t new_fraction_bits = fraction_bits;
        using new_value_type =
            value_type_for<new_integer_bits,
                           new_fraction_bits,
                           ArgT::is_signed or is_signed>::type;
        using type =
            qnumber<new_integer_bits, new_fraction_bits, new_value_type>;
        auto lhs_raw = static_cast<new_value_type>(raw());
        auto rhs_raw = static_cast<new_value_type>(argument.raw());
        auto result  = static_cast<new_value_type>(lhs_raw + rhs_raw);
        return type{as_is_t{result}};
    }

    template <qformatted ArgT>
        requires common_denominator<qnumber, ArgT>
    [[nodiscard]] constexpr auto subtract(ArgT argument) const noexcept
    {
        constexpr size_t new_integer_bits =
            std::max(ArgT::integer_bits, integer_bits) + 1;
        constexpr size_t new_fraction_bits = fraction_bits;
        using new_value_type =
            minimal_signed_type<new_integer_bits, new_fraction_bits>;
        using type =
            qnumber<new_integer_bits, new_fraction_bits, new_value_type>;
        auto lhs_raw = static_cast<new_value_type>(raw());
        auto rhs_raw = static_cast<new_value_type>(argument.raw());
        auto result  = static_cast<new_value_type>(lhs_raw - rhs_raw);
        return type{as_is_t{result}};
    }

    [[nodiscard]] constexpr auto operator-() const noexcept
    {
        using new_value_type = minimal_signed_type<integer_bits, fraction_bits>;
        return qnumber<integer_bits, fraction_bits, new_value_type>{
            as_is_t<new_value_type>(raw() * -1)};
    }

    template <std::floating_point ArgT>
    [[nodiscard]] constexpr bool is_nearest_to(ArgT argument) const noexcept
    {
        [[maybe_unused]] auto this_as_floating = this->as<ArgT>();
        auto clamped =
            std::clamp(argument,
                       std::numeric_limits<qnumber>::lowest().as<ArgT>(),
                       std::numeric_limits<qnumber>::max().as<ArgT>());
        qnumber argument_as_q{clamped};

        if (raw() == std::numeric_limits<value_type>::lowest())
        {
            auto higher = raw() + 1;
            return argument_as_q.raw() <= higher;
        }
        else if (raw() == std::numeric_limits<value_type>::max())
        {
            auto lower = raw() - 1;
            return lower <= argument_as_q.raw();
        }
        else
        {
            auto lower  = raw() - 1;
            auto higher = raw() + 1;
            return lower <= argument_as_q.raw() and
                   argument_as_q.raw() <= higher;
        }
        // return argument_as_q.raw() == raw();
    }

    template <qformatted SaturateIntoT, qformatted ArgT>
    [[nodiscard]] constexpr SaturateIntoT saturate_mul(
        ArgT multiplicant) const noexcept
    {
        // need implementation
    }
};

template <size_t IntegerBits, size_t FractionBits>
using qu = qnumber<IntegerBits,
                   FractionBits,
                   minimal_unsigned_type<IntegerBits, FractionBits>>;
template <size_t IntegerBits, size_t FractionBits>
using qs = qnumber<IntegerBits,
                   FractionBits,
                   minimal_signed_type<IntegerBits, FractionBits>>;

// shouldn't be possible to alias memory with these q types, not sure if proper
// trait here static_assert(!std::is_trivial<qs<1, 1>>);qu<1, 7>
static_assert(qformatted<qu<1, 7>>);
static_assert(qformatted<qs<0, 7>>);
static_assert(std::is_trivially_copyable_v<qs<1, 1>>);
static_assert(std::copy_constructible<qs<1, 1>>);
static_assert(std::assignable_from<qs<1, 1>&, qs<1, 1>>);
static_assert(safely_convertible<qu<1, 7>, qs<2, 7>>);
static_assert(
    std::same_as<decltype(std::declval<qu<1, 7>>().as<qs<2, 7>>()), qs<2, 7>>);
} // namespace bit

namespace std
{
template <bit::qformatted T> class numeric_limits<T>
{
  public:
    static constexpr bool is_specialized = true;

    static constexpr T min() noexcept
    {
        typename T::value_type value{};
        if constexpr (T::is_signed)
        {
            bit::clear_sign_bit(value);
        }
        bit::clear_bits<T::msb, T::lsb>(value);
        bit::set_bit<0>(value);
        return {bit::as_is_t{value}};
    }

    static constexpr T max() noexcept
    {
        typename T::value_type value{};
        if constexpr (is_signed)
        {
            bit::clear_sign_bit(value);
        }
        bit::set_bits<T::msb, T::lsb>(value);
        return T{bit::as_is_t{value}};
    }

    static constexpr T lowest() noexcept
    {
        typename T::value_type value{};
        if constexpr (is_signed)
        {
            bit::set_sign_bit(value);
            constexpr auto container_bit_width =
                bit::width<typename T::value_type>();
            bit::set_bits<container_bit_width, 0>(value);
            bit::clear_bits<T::msb, T::lsb>(value);
        }
        return {bit::as_is_t{value}};
    }

    static constexpr T epsilon() noexcept
    {
        return min();
    }

    static constexpr int digits = T::integer_bits + T::fraction_bits;
    // static constexpr int digits10     = std::log10(digits);
    static constexpr int max_digits10 = digits;

    static constexpr bool is_signed  = T::is_signed;
    static constexpr bool is_integer = T::fraction_bits > 0;
    static constexpr bool is_exact =
        T::fraction_bits >
        0; // if no fraction bits, then qnumber becomes plain integer

    static constexpr bool has_infinity      = false;
    static constexpr bool has_quiet_NaN     = false;
    static constexpr bool has_signaling_NaN = false;
    // static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;

    // static constexpr qnumber infinity() noexcept { return qnumber(); }
    // static constexpr qnumber quiet_NaN() noexcept { return qnumber(); }
    // static constexpr qnumber signaling_NaN() noexcept { return qnumber(); }
    // static constexpr qnumber denorm_min() noexcept { return qnumber(); }

    static constexpr bool is_iec559  = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo  = false;

    static constexpr bool traps                    = false;
    static constexpr bool tinyness_before          = false;
    static constexpr float_round_style round_style = round_toward_zero;
};
} // namespace std

static_assert(std::numeric_limits<bit::qu<8, 0>>::max().raw() == 255);
static_assert(std::numeric_limits<bit::qu<8, 0>>::lowest().raw() == 0);
static_assert(std::numeric_limits<bit::qu<8, 0>>::min().raw() == 1);

static_assert(std::numeric_limits<bit::qu<7, 0>>::max().raw() == 127);
static_assert(std::numeric_limits<bit::qu<7, 0>>::lowest().raw() == 0);
static_assert(std::numeric_limits<bit::qu<7, 0>>::min().raw() == 1);

static_assert(std::numeric_limits<bit::qs<8, 0>>::max().raw() == 255);
static_assert(std::numeric_limits<bit::qs<8, 0>>::lowest().raw() == -256);
static_assert(std::numeric_limits<bit::qs<8, 0>>::min().raw() == 1);

static_assert(std::numeric_limits<bit::qs<7, 0>>::max().raw() == 127);
static_assert(std::numeric_limits<bit::qs<7, 0>>::lowest().raw() == -128);
static_assert(std::numeric_limits<bit::qs<7, 0>>::min().raw() == 1);
