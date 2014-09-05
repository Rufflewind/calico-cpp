#ifndef ZJLKEPGDENDJHYVPNFMB
#define ZJLKEPGDENDJHYVPNFMB
/// @file
///
/// Towards better and safer memory management.
///
#include <cassert>
#include <cstddef>
#include <cstring>
#include <limits>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <utility>
#include <type_traits>

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#if __GXX_RTTI || __has_feature(cxx_rtti)
# undef  CAL_HAVE_RTTI
# define CAL_HAVE_RTTI
#endif

#if defined(__GXX_EXPERIMENTAL_CXX0X__)                 \
    && (__GNUC__ >  4 ||                                \
        __GNUC__ == 4 && (__GNUC_MINOR__ >  8 ||        \
                          __GNUC_MINOR__ == 8 &&        \
                          __GNUC_PATCHLEVEL__ > 1))     \
    || __has_feature(cxx_reference_qualified_functions)
# undef  CAL_HAVE_REFERENCE_QUALIFIED_FUNCTIONS
# define CAL_HAVE_REFERENCE_QUALIFIED_FUNCTIONS
#endif

#undef  CAL_RVQF
#ifdef CAL_HAVE_REFERENCE_QUALIFIED_FUNCTIONS
# define CAL_RVQF &&
#else
# define CAL_RVQF
#endif

#undef CAL_NOEXCEPT_ND
#ifndef NDEBUG
# define CAL_NOEXCEPT_ND noexcept
#else
# define CAL_NOEXCEPT_ND
#endif

namespace cal {

#ifndef CALICO_DOC_ONLY
// for some reason this causes Doxygen produce duplicates
template<class T>
class slice;
#endif

template<class T>
slice<T> slice_from_raw_parts(T* pointer, std::size_t size) CAL_NOEXCEPT_ND;

/// A non-owning reference to an array of elements.
template<class T>
class slice {
public:

    /// Type of the elements.
    typedef T value_type;

    /// Iterator type.
    typedef value_type* iterator;

    /// Reference type.
    typedef value_type& reference;

    /// Size type.
    typedef std::size_t size_type;

    /// Constructs a slice of size zero.
    slice() noexcept
        : _ptr(),
          _size() {}

    /// Copies a slice.
    ///
    /// @note The slice shares the memory with the original.
    slice(const slice& other) noexcept = default;

    /// Moves from another slice.
    ///
    /// The other slice is reset to an empty slice.
    slice(slice&& other) noexcept { *this = std::move(other); }

    /// Copies a slice.
    ///
    /// @note The slice shares the memory with the original.
    slice& operator=(const slice& other) noexcept = default;

    /// Moves from another slice.
    ///
    /// The other slice is reset to an empty slice.
    slice operator=(slice&& other) noexcept {
        _ptr  = other._ptr;
        _size = other._size;
        other._size = 0;
        other._ptr  = nullptr;
        return *this;
    }

    /// Returns a slice element.
    reference operator[](size_type index) const CAL_NOEXCEPT_ND {
#ifndef NDEBUG
        if (index >= size()) {
            std::ostringstream s;
            s << "slice::operator[]: index out of range [index = "
              << index << ", size = " << size() << "]";
            throw std::runtime_error(s.str());
        }
#endif
        return begin()[index];
    }

    /// Returns a slice element.
    reference operator()(size_type index) const CAL_NOEXCEPT_ND {
        return (*this)[index];
    }

    /// Iterator to the first element.
    iterator begin() const noexcept { return _ptr; }

    /// Iterator to the past-the-end element.
    iterator end() const noexcept { return begin() + size(); }

    /// Returns the length of the slice.
    size_type size() const noexcept { return _size; }

    /// Returns a copy of itself.
    slice to_slice() const noexcept { return *this; }

    /// Returns a slice from `start` to the end.
    ///
    /// The index is clamped within the bounds of the slice.
    slice to_slice(size_type start) const noexcept {
        return to_slice(start, size());
    }

    /// Returns a slice from `start` to `stop`.
    ///
    /// The indices are clamped within the bounds of the slice.
    slice to_slice(size_type start, size_type stop) const noexcept {
        stop  = std::min(stop, size());
        start = std::min(start, stop);
        return slice(begin() + start, stop - start);
    }

    /// Returns an immutable version of itself.
    slice<const value_type> to_const() const noexcept {
        return slice_from_raw_parts(
            static_cast<const value_type*>(begin()),
            size()
        );
    }

private:

    static constexpr bool _can_initialize_with_memset =
        std::is_integral<value_type>::value ||
        (std::is_floating_point<value_type>::value &&
         std::numeric_limits<value_type>::is_iec559);

    // this is more restrictive than necessary, but unfortunately
    // `is_trivially_copyable` isn't implemented in many compilers
    static constexpr bool _can_copy_with_memcpy =
        std::is_scalar<value_type>::value;

    static constexpr bool _noexcept_constructor =
        noexcept(value_type());

    static constexpr bool _noexcept_destructor =
        noexcept(std::declval<value_type>().~value_type());

public:

    /// Clears the slice.
    ///
    /// @note     `value_type` must be non-const and `DefaultConstructible`.
    void clear()
    noexcept(_can_initialize_with_memset ||
             _noexcept_constructor && _noexcept_destructor) {
        if (_can_initialize_with_memset) {
            std::memset(begin(), 0, size() * sizeof(value_type));
        } else {
            for (auto& i : *this) {
                i.~value_type();
                new (&i) value_type();
            }
        }
    }

    /// Copies elements from another slice.
    ///
    /// @return   The number of elements copied, which is the smaller of the
    ///           two slices.
    size_type copy_from(slice<const value_type> other) const
    noexcept(_can_copy_with_memcpy) {
        const size_type min_size = std::min(size(), other.size());
        const value_type* const src_begin  = other.begin();
        const value_type* const src_end = other.begin() + min_size;
        value_type* const dest_begin = begin();
        if (_can_copy_with_memcpy)
            copy_trivially_from(other);
        else if (src_begin < dest_begin && dest_begin < src_end)
            std::copy_backward(src_begin, src_end, end());
        else
            std::copy(src_begin, src_end, dest_begin);
        return min_size;
    }

    /// [Unsafe] Performs a raw copy of the elements from another slice.
    ///
    /// @return   The number of elements copied, which is the smaller of the
    ///           two slices.
    ///
    /// @warning  If `value_type` is not trivially copyable, the behavior is
    ///           undefined.
    size_type copy_trivially_from(slice<const value_type> other) const
    noexcept {
        const size_type min_size = std::min(size(), other.size());
        std::memmove(begin(), other.begin(), min_size * sizeof(value_type));
        return min_size;
    }

    /// Calls the default constructor of every element.
    ///
    /// @note     `value_type` must be non-const and `DefaultConstructible`.
    ///           This can cause resource leaks if the elements have already
    ///           been initialized.
    void initialize() const noexcept(_noexcept_constructor) {
        for (auto& i : *this)
            new (&i) value_type();
    }

    /// [Unsafe] Calls the destructor of every element.
    ///
    /// @note     `value_type` must be non-const and `Destructible`.
    ///
    /// @warning  Be careful: if the array is owned by another object, this
    ///           could result in the destructor being called twice, which
    ///           can cause undefined behavior.
    void deinitialize() const noexcept(_noexcept_destructor) {
        for (auto& i : *this)
            i.~value_type();
    }

private:

    friend slice slice_from_raw_parts<value_type>
        (value_type* pointer, size_type size) CAL_NOEXCEPT_ND;

    value_type* _ptr;
    size_type _size;

    slice(value_type* pointer, size_type size) noexcept
        : _ptr(pointer),
          _size(size) {}
};

/// [Unsafe] Constructs a new slice from raw pointers.
///
/// @warning   The slice does not take ownership of the memory.
template<class T>
slice<T> slice_from_raw_parts(T* pointer, std::size_t size) CAL_NOEXCEPT_ND {
#ifndef NDEBUG
    if ((size == 0) != (pointer == nullptr)) {
        std::ostringstream s;
        s << "slice_from_raw_parts: bad pointer and size combination"
             " [pointer = " << pointer << ", size = " << size << "]";
        throw std::runtime_error(s.str());
    }
#endif
    return slice<T>(pointer, size);
}

/// [Unsafe] Constructs a new slice from raw pointers.
///
/// @warning   The slice does not take ownership of the memory.
template<class T>
slice<T> slice_from_raw_parts(T* begin, T* end) CAL_NOEXCEPT_ND {
#ifndef NDEBUG
    if (begin > end) {
        std::ostringstream s;
        s << "slice::slice: `begin` is after `end` [begin = "
          << begin << ", end = " << end << "]";
        throw std::runtime_error(s.str());
    }
#endif
    return slice_from_raw_parts(begin, static_cast<std::size_t>(end - begin));
}

/// [Unsafe] Constructs a new slice from a reference to a fixed-size array.
///
/// @warning   The slice does not take ownership of the memory.
template<class T, std::size_t N>
slice<T> slice_from_array(T (& array)[N]) noexcept {
    return slice_from_raw_parts(array, N);
}

/// Compares two slices using shallow equality: they are equal if and only if
/// the slices reference the same memory locations.
template<class T>
bool operator==(slice<T> left, slice<T> right) {
    return left.begin() == right.begin() && left.size() == right.size();
}

/// Compares two slices using shallow equality: they are equal if and only if
/// the slices reference the same memory locations.
template<class T>
bool operator!=(slice<T> left, slice<T> right) {
    return !(left == right);
}

/// Compares two slices using deep equality: they are equal if and only if the
/// individual elements are all equal.
template<class T>
bool equal(slice<T> left, slice<T> right) {
    if (left.size() != right.size())
        return false;
    for (std::size_t i = 0; i != left.size(); ++i)
        if (left[i] != right[i])
            return false;
    return true;
}

/// An owned, dynamic array of elements.
template<class T>
class vec {
public:

    /// Type of the elements.
    typedef T value_type;

    /// Size type.
    typedef std::size_t size_type;

    /// Creates an empty array.
    vec() noexcept
        : _cap(),
          _size() {}

    /// Creates an array with size elements.
    ///
    /// If `value_initialize` is `true`, the new elements are
    /// value-initialized.  Otherwise, they are default-initialized.
    ///
    /// On failure, raises `std::runtime_error` (not `std::bad_alloc`).
    vec(size_type size, bool value_initialize = true)
        : _cap(),
          _size(size),
          _buf(_alloc(size, value_initialize)) {}

    /// Increases the capacity of the container.
    ///
    /// This will never cause the capacity to decrease.
    void reserve(size_type capacity) {
        if (_cap > capacity)
            return;
        _buf.reset(_alloc(capacity, false));
        _cap = capacity;
    }

    /// Resizes the vector.
    ///
    /// If `value_initialize` is `true`, the new elements are
    /// value-initialized.  Otherwise, they are default-initialized.
    void resize(size_type new_size, bool value_initialize = true) {
        assert(_cap >= _size);
        if (_cap != new_size) {
            _buf.reset(_alloc(new_size, false));
            _cap = new_size;
        }
        _size = new_size;
    }

    /// Resizes the vector, avoiding reallocation if the new size is smaller
    /// than or equal to the current capacity.
    ///
    /// If `value_initialize` is `true`, the new elements are
    /// value-initialized.  Otherwise, they are default-initialized.
    void resize_fast(size_type new_size) {
        assert(_cap >= _size);
        if (_cap < new_size) {
            size_type new_cap = _cap;
            while (new_cap *= 2 < new_size);
            _buf.reset(_alloc(new_cap, false));
            _cap = new_cap;
        }
        _size = new_size;
    }

    /// Converts the array into a `std::unique_ptr`.
    ///
    /// @note     This will cause the array to become empty.
    std::unique_ptr<value_type[]> as_unique_ptr() CAL_RVQF noexcept {
        _cap  = 0;
        _size = 0;
        return std::move(_buf);
    }

    /// [Unsafe] Obtain a slice of the entire vector.
    ///
    /// @warning   The slice does not take ownership of the memory.  Any
    ///            operation that modifies the vector can invalidate the
    ///            slice.
    slice<value_type> to_slice() const noexcept {
        return slice_from_raw_parts(_buf.get(), _size);
    }

private:

    size_type _cap;
    size_type _size;
    std::unique_ptr<value_type[]> _buf;

    // Allocates an array with `count` elements.
    //
    // If `value_initialize` is `true`, the new elements are
    // value-initialized.  Otherwise, they are default-initialized.
    static T* _alloc(std::size_t count, bool value_initialize = true) {
        T* ptr = value_initialize
            ? new (std::nothrow) T[count]()
            : new (std::nothrow) T[count];
        if (ptr)
            return ptr;
        std::ostringstream s;
        s << "vec::_alloc: can't allocate [count = " << count
#ifdef CAL_HAVE_RTTI
          << ", type = " << typeid(value_type).name()
#endif
          << "]";
        throw std::runtime_error(s.str());
    }
};

}
#endif
