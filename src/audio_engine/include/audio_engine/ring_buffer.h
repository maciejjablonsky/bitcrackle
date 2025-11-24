#pragma once

#include <exception>
#include <memory_resource>
#include <ranges>
#include <type_traits>

namespace bit
{
template <typename T> class ring_buffer
{
  private:
    std::pmr::polymorphic_allocator<> _allocator =
        std::pmr::get_default_resource();
    T* _start = nullptr;
    T* _end   = nullptr;
    size_t _read_index{};
    size_t _write_index{};
    size_t _size{};
    size_t _alignment{};

  public:
    ring_buffer() = default;

    explicit ring_buffer(std::pmr::polymorphic_allocator<> allocator) noexcept
        : _allocator(allocator)
    {
    }

    explicit ring_buffer(size_t capacity,
                         size_t alignment = std::alignment_of_v<T>,
                         std::pmr::polymorphic_allocator<> allocator = {})
        : _allocator(allocator), _alignment(alignment)
    {
        auto allocation =
            _allocator.allocate_bytes(capacity * sizeof(T), _alignment);
        if (not allocation)
        {
            throw std::bad_alloc{};
        }
        _start = static_cast<T*>(allocation);
        _end   = _start + capacity;
    }

    ~ring_buffer()
    {
        if (_start)
        {
            _allocator.deallocate_bytes(_start, _end - _start, _alignment);
        }
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return _size == 0;
    }

    [[nodiscard]] constexpr std::size_t capacity() const noexcept
    {
        return _end - _start;
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept
    {
        return _size;
    }

    template <std::ranges::forward_range R>
    constexpr void push(R&& values) noexcept
    {
        for (const auto& v : values)
        {
            *(_start + _write_index) = v;
            _write_index             = (_write_index + 1) % capacity();
            if (_size == capacity())
            {
                _read_index = (_read_index + 1) % capacity();
            }
            _size = std::min(capacity(), _size + 1);
        }
    }

    T* begin() noexcept
    {
        return _start + _read_index;
    }

    T* end() noexcept
    {
        return _start + (_read_index + _size) % capacity();
    }
};
} // namespace bit
