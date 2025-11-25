#include <audio_engine/ring_buffer.h>

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <memory_resource>
#include <ranges>

TEST_CASE("Default constructed ring_buffer is empty with 0 capacity",
          "[audio_engine|ring_buffer]")
{
    bit::ring_buffer<int> ring;
    CHECK(ring.empty());
    CHECK(ring.capacity() == 0);
    CHECK(ring.size() == 0);
}

TEST_CASE("ring_buffer is constructible with std::pmr::polymorphic_allocator",
          "[audio_engine|ring_buffer]")
{
    std::pmr::polymorphic_allocator allocator;
    bit::ring_buffer<int> ring(allocator);
    CHECK(ring.empty());
    CHECK(ring.capacity() == 0);
    CHECK(ring.size() == 0);
}

class measure_memory_resource : public std::pmr::monotonic_buffer_resource
{
  protected:
    size_t _sum_bytes{};

    void* do_allocate(std::size_t bytes, std::size_t alignment) override
    {
        _sum_bytes += bytes;
        return monotonic_buffer_resource::do_allocate(bytes, alignment);
    }

  public:
    virtual ~measure_memory_resource() = default;
    size_t how_many_allocated() const noexcept
    {
        return _sum_bytes;
    }
};

TEST_CASE(
    "ring_buffer with size given at construction allocates buffer of size",
    "[audio_engine|ring_buffer]")
{
    measure_memory_resource resource;
    std::pmr::polymorphic_allocator allocator(&resource);
    constexpr size_t elements = 24;
    bit::ring_buffer<int> ring(elements, std::alignment_of_v<int>, allocator);
    CHECK(ring.empty());
    CHECK(ring.size() == 0);
    CHECK(ring.capacity() == elements);
    CHECK(resource.how_many_allocated() == sizeof(int) * elements);
}

template <std::ranges::input_range LeftR, std::ranges::input_range RightR>
void ranges_equal(const LeftR& a, const RightR& b)
{
    REQUIRE(a.size() == b.size());

    int i{};
    for (auto&& pair : std::views::zip(a, b))
    {
        auto&& [left, right] = pair;
        INFO("Index: " << i);
        REQUIRE(left == right);
        ++i;
    }
}

SCENARIO("adding elements to ring_buffer", "[audio_engine|ring_buffer]")
{
    GIVEN("ring_buffer<int> with 10 elements")
    {
        bit::ring_buffer<int> ring(10);

        WHEN("pushed first 20 positive integers")
        {
            auto values = std::views::iota(0, 20);
            ring.push(values);

            THEN("ring_buffer contains last 10 values")
            {
                ranges_equal(ring, values | std::views::drop(10));
            }
        }

        WHEN("pushed first 15 positive integers")
        {
            ring.push(std::views::iota(0, 15));

            THEN("ring_buffer contains last 10 values")
            {
                ranges_equal(ring,
                             std::views::iota(0, 15) | std::views::drop(5));
            }
        }
    }
}
