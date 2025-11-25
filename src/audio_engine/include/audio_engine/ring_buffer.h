#pragma once

#include <cassert>
#include <exception>
#include <memory_resource>
#include <ranges>
#include <type_traits>
#include <utility>

namespace bit
{
template <typename T> class ring_buffer
{
  public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;

    struct iterator
    {
        ring_buffer* _parent  = nullptr;
        std::ptrdiff_t _index = 0;

        using difference_type = std::ptrdiff_t;
        using value_type      = typename ring_buffer::value_type;
        using reference       = value_type&;

        constexpr iterator() noexcept = default;
        constexpr iterator(ring_buffer& parent,
                           std::ptrdiff_t index = 0) noexcept
            : _parent(&parent), _index(index)
        {
            assert(_parent != nullptr);
        }

        constexpr bool operator<=>(const iterator&) const noexcept = default;

        constexpr reference operator*() noexcept
        {
            assert(_parent->capacity() > 0);
            return *(_parent->_start + (_index % _parent->capacity()));
        }

        constexpr reference operator*() const noexcept
        {
            assert(_parent->capacity() > 0);
            return *(_parent->_start + (_index % _parent->capacity()));
        }

        constexpr iterator& operator++() noexcept
        {
            ++_index;
            return *this;
        }

        constexpr iterator& operator--() noexcept
        {
            --_index;
            return *this;
        }

        constexpr iterator operator++(int) const noexcept
        {
            auto tmp = *this;
            this->operator++();
            return tmp;
        }

        constexpr iterator operator--(int) const noexcept
        {
            auto tmp = *this;
            this->operator--();
            return tmp;
        }

        constexpr difference_type operator-(
            const iterator& other) const noexcept
        {
            return _index - other._index;
        }

        constexpr iterator& operator+=(difference_type advance) noexcept
        {
            _index += advance;
            return *this;
        }

        constexpr iterator operator+(difference_type advance) const noexcept
        {
            auto tmp = *this;
            tmp += advance;
            return tmp;
        }

        constexpr iterator& operator-=(difference_type advance) noexcept
        {
            _index -= advance;
            return *this;
        }

        constexpr iterator operator-(difference_type advance) const noexcept
        {
            auto tmp = *this;
            tmp -= advance;
            return tmp;
        }

        constexpr reference operator[](difference_type index) noexcept
        {
            auto tmp = *this;
            tmp += index;
            return *tmp;
        }

        constexpr reference operator[](difference_type) const noexcept
        {
            auto tmp = *this;
            tmp += index;
            return *tmp;
        }
    };

    constexpr friend iterator operator+(iterator::difference_type n,
                                        const iterator& iter) noexcept
    {
        return iter + n;
    }

    static_assert(std::copyable<iterator>);
    static_assert(std::random_access_iterator<iterator>);

    struct const_iterator
    {
        ring_buffer* _parent  = nullptr;
        std::ptrdiff_t _index = 0;

        using difference_type = std::ptrdiff_t;
        using value_type      = typename ring_buffer::value_type;
        using const_reference = const value_type&;

        constexpr const_iterator() noexcept = default;
        constexpr const_iterator(ring_buffer& parent,
                                 std::ptrdiff_t index = 0) noexcept
            : _parent(&parent), _index(index)
        {
            assert(_parent != nullptr);
        }
        constexpr const_iterator(const iterator& other) noexcept
            : _parent(other._parent), _index(other._index)
        {
        }

        constexpr bool operator<=>(const const_iterator&) const noexcept =
            default;

        const_reference operator*() const noexcept
        {
            assert(_parent->capacity() > 0);
            return *(_parent->_start + (_index % _parent->capacity()));
        }

        const_reference operator*() noexcept
        {
            assert(_parent->capacity() > 0);
            return *(_parent->_start + (_index % _parent->capacity()));
        }

        const_iterator& operator++() noexcept
        {
            ++_index;
            return *this;
        }

        const_iterator operator++(int) const noexcept
        {
            auto tmp = *this;
            this->operator++();
            return tmp;
        }

        const_iterator& operator--() noexcept
        {
            --_index;
            return *this;
        }

        const_iterator operator--(int) const noexcept
        {
            auto tmp = *this;
            this->operator--();
            return tmp;
        }

        constexpr difference_type operator-(
            const const_iterator& other) const noexcept
        {
            return _index - other._index;
        }

        constexpr const_iterator& operator+=(difference_type advance) noexcept
        {
            _index += advance;
            return *this;
        }

        constexpr const_iterator operator+(
            difference_type advance) const noexcept
        {
            auto tmp = *this;
            tmp += advance;
            return tmp;
        }

        constexpr const_iterator& operator-=(difference_type advance) noexcept
        {
            _index -= advance;
            return *this;
        }

        constexpr const_iterator operator-(
            difference_type advance) const noexcept
        {
            auto tmp = *this;
            tmp -= advance;
            return tmp;
        }

        constexpr const_reference operator[](
            difference_type index) const noexcept
        {
            auto tmp = *this;
            tmp += index;
            return *tmp;
        }
    };

    constexpr friend const_iterator operator+(
        const_iterator::difference_type n, const const_iterator& iter) noexcept
    {
        return iter + n;
    }
    static_assert(std::random_access_iterator<const_iterator>);
    static_assert(std::copyable<const_iterator>);
    static_assert(std::constructible_from<const_iterator, iterator>);
    // static_assert(std::weakly_incrementable<const_iterator>);

  private:
    std::pmr::polymorphic_allocator<> _allocator =
        std::pmr::get_default_resource();
    T* _start = nullptr;
    T* _end   = nullptr;
    iterator _write_index{*this};
    iterator _read_index{*this};
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
            *_write_index = v;
            ++_write_index;
            if (_size == capacity())
            {
                ++_read_index;
            }
            _size = std::min(capacity(), _size + 1);
        }
    }

    iterator begin() noexcept
    {
        return _read_index;
    }

    iterator end() noexcept
    {
        return _read_index + _size;
    }

    [[nodiscard]] const_iterator begin() const noexcept
    {
        return _read_index;
    }

    [[nodiscard]] const_iterator end() const noexcept
    {
        return _read_index + _size;
    }
};
static_assert(std::ranges::range<ring_buffer<int>>);
} // namespace bit
