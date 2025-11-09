#pragma once

#include "math/utilities.h"

#include <bit>
#include <cassert>
#include <concepts>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bit
{
template <std::signed_integral T> consteval size_t width() noexcept
{
    return sizeof(T) * 8 - 1;
}

template <std::unsigned_integral T> consteval size_t width() noexcept
{
    return sizeof(T) * 8;
}

static_assert(width<int8_t>() == 7);
static_assert(width<uint8_t>() == 8);
static_assert(width<int64_t>() == 63);
static_assert(width<uint64_t>() == 64);

template <typename T, size_t... Bits>
[[nodiscard]] constexpr T generate_bitmask_from_sequence(
    std::index_sequence<Bits...>) noexcept
{
    static_assert(std::is_integral_v<T>,
                  "Mask type is expected to be integral.");
    return ((T{1} << Bits) | ...);
}

template <std::integral T>
[[nodiscard]] constexpr bool contains_bit(std::integral auto index) noexcept
{
    return sizeof(T) * 8 > static_cast<size_t>(index);
}

static_assert(!contains_bit<uint8_t>(-1));
static_assert(contains_bit<uint8_t>(0));
static_assert(contains_bit<uint8_t>(7));
static_assert(!contains_bit<uint8_t>(8));

template <typename T, size_t MSBitIndex, size_t LSBitIndex>
concept contains_bit_range =
    MSBitIndex >= LSBitIndex and contains_bit<T>(MSBitIndex) and
    contains_bit<T>(LSBitIndex);

template <size_t MSBitIndex, size_t LSBitIndex, std::integral T>
    requires contains_bit_range<T, MSBitIndex, LSBitIndex>
[[nodiscard]] constexpr T generate_bitmask() noexcept
{
    constexpr size_t NumBits = MSBitIndex - LSBitIndex + 1;
    return GenerateBitMaskFromSequence<T>(std::make_index_sequence<NumBits>{},
                                          LSBitIndex);
}

// helper overload to shift indices up by LSBitIndex
template <typename T, size_t... Offsets>
[[nodiscard]] constexpr T GenerateBitMaskFromSequence(
    std::index_sequence<Offsets...>, size_t Base) noexcept
{
    return as<T>(((as<T>(1) << (Base + Offsets)) | ...));
}

static_assert(generate_bitmask<3, 0, uint8_t>() == 0b0000'1111);
static_assert(generate_bitmask<7, 4, uint8_t>() == 0b1111'0000);

template <size_t MSBitIndex, size_t LSBitIndex, std::integral T>
constexpr void set_bits(T& out_value) noexcept
{
    constexpr auto mask = generate_bitmask<MSBitIndex, LSBitIndex, T>();
    out_value |= mask;
}

template <size_t BitIndex>
constexpr void set_bit(std::integral auto& out_value) noexcept
{
    set_bits<BitIndex, BitIndex>(out_value);
}

template <size_t MSBitIndex, size_t LSBitIndex, std::integral T>
constexpr void clear_bits(T& out_value) noexcept
{
    constexpr auto mask = ~generate_bitmask<MSBitIndex, LSBitIndex, T>();
    out_value &= mask;
}

template <size_t BitIndex>
constexpr void clear_bit(std::integral auto& out_value) noexcept
{
    clear_bits<BitIndex, BitIndex>(out_value);
}

template <std::signed_integral T> constexpr size_t sign_bit_index = width<T>();

static_assert(sign_bit_index<int8_t> == 7);
static_assert(sign_bit_index<int64_t> == 63);

template <std::signed_integral T>
constexpr void set_sign_bit(T& out_value) noexcept
{
    set_bit<sign_bit_index<T>>(out_value);
}

template <std::signed_integral T>
constexpr void clear_sign_bit(T& out_value) noexcept
{
    clear_bit<sign_bit_index<T>>(out_value);
}

static_assert([] {
    uint8_t value = 0;
    set_bits<7, 4>(value);
    return value;
}() == 0b1111'0000);

static_assert([] {
    uint8_t value = 0;
    set_bits<5, 3>(value);
    return value;
}() == 0b0011'1000);

static_assert([] {
    uint8_t value = 0xFF;
    clear_bits<7, 4>(value);
    return value;
}() == 0b0000'1111);

template <size_t MSBitIndex, size_t LSBitIndex, std::integral T>
    requires contains_bit_range<T, MSBitIndex, LSBitIndex>
[[nodiscard]] constexpr std::unsigned_integral auto extract_bits(
    T value) noexcept
{
    using unsigned_value_type = std::make_unsigned_t<T>;
    auto unsigned_value       = std::bit_cast<unsigned_value_type>(value);
    constexpr auto mask       = generate_bitmask<MSBitIndex, LSBitIndex, T>();
    unsigned_value = static_cast<unsigned_value_type>(unsigned_value & mask);
    unsigned_value =
        static_cast<unsigned_value_type>(unsigned_value >> (LSBitIndex));
    return unsigned_value;
}

static_assert(extract_bits<5, 3>(0b0011'1000) == 0b111);
static_assert(extract_bits<6, 4>(0b0111'0000) == 0b111);

template <size_t BitIndex, std::integral T>
    requires contains_bit<T, BitIndex>
[[nodiscard]] constexpr bool test_bit(T value) noexcept
{
    return as<bool>(extract_bits<BitIndex, BitIndex>(value));
}

template <size_t MSBitIndex, size_t LSBitIndex, std::integral T>
constexpr void assign_bits(T& destination, std::unsigned_integral auto value)
    requires contains_bit_range<T, MSBitIndex, LSBitIndex>
{
    if (std::is_constant_evaluated())
    {
        if (as<size_t>(std::bit_width(value)) > (MSBitIndex - LSBitIndex + 1))
        {
            throw std::range_error("value won't fit on given bit range");
        }
    }
    else
    {
        assert(as<size_t>(std::bit_width(value)) <=
               (MSBitIndex - LSBitIndex + 1));
    }

    constexpr auto mask = ~generate_bitmask<MSBitIndex, LSBitIndex, T>();
    auto unsigned_destination =
        std::bit_cast<std::make_unsigned_t<T>>(destination);
    unsigned_destination &= mask;
    unsigned_destination |= (as<T>(value) << LSBitIndex);
    destination = std::bit_cast<T>(unsigned_destination);
}

static_assert([] {
    auto value = 0;
    assign_bits<5, 3>(value, 0b101u);
    return value;
}() == 0b0010'1000);

static_assert([] {
    auto value = 0b0011'0100;
    assign_bits<5, 3>(value, 0b011u);
    return value;
}() == 0b0001'1100);

} // namespace bit
