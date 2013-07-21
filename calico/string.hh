#ifndef FZ_STRING_HH
#define FZ_STRING_HH
#include <iterator>
#include <sstream>
#include <string>
#include "cxx11.hh"
namespace fz {

/// Constructs a string representation of an object using the stream insertion
/// operator (`<<`).
template<class T>
inline std::string to_string(const T& x) {
    std::ostringstream stream;
    stream << x;
    return stream.str();
}

/// Returns a copy of the string.
inline std::string to_string(const std::string& s) { return s; }

/// Converts a null-terminated string to an `std::string`.
inline std::string to_string(const char* s) { return s; }

/// Converts a character to a string.
inline std::string to_string(char c) { return std::string(1, c); }

namespace _priv {
template<class T, class String>
inline T parse(const String& s) {
    T x;
    std::istringstream stream(s);
    stream >> x;
    return x;
}
}

/// Extracts an object of type `T` from a string using the stream extraction
/// operator (`>>`).
template<class T>
inline T parse(const std::string& s) { _priv::parse<T>(s); }

/// Extracts an object of type `T` from a string using the stream extraction
/// operator (`>>`).
template<class T>
inline T parse(const char* s) { _priv::parse<T>(s); }

//////////////////////////////////////////////////////////////////////////////
// Null-terminated iterator

/// A `ForwardIterator` that traverses a null-terminated array.
///
/// @tparam T    The element type of the array.  Must be
///              `DefaultConstructible`, `Destructible`, and
///              `EqualityComparable`.
///
/// A *null-terminated array* is an array of elements in which the
/// past-the-end element is determined by a default-constructed value of the
/// element type.  Elements before the past-the-end element are forbidden to
/// have the same value as the default-constructed value.  A typical example
/// would be a null-terminated array of `char`s (a plain C string).
///
/// This iterator allows forward iteration from the first value of the array
/// to the last value of the array, just before the past-the-end element.
/// Elements beyond the past-the-end element are not considered to be part of
/// the null-terminated array.
///
/// The construction of this iterator does not require the position of the
/// past-the-end element and thus it has O(1) complexity with respect to the
/// length of the string.
///
/// The following actions will invoke undefined behavior:
///
/// - Traversing beyond the range of the null-terminated array.
///
/// - Dereferencing the past-the-end iterator.
///
/// - Comparing equality or inequality with iterators constructed from
///   different pointers.
///
/// - Using the iterator in any way after the array has been invalidated.
///
template<class T>
class null_terminated_iterator {
public:

    /// Iterator category.
    typedef std::forward_iterator_tag iterator_category;

    /// Value type.
    typedef T value_type;

    /// Pointer type.
    typedef value_type* pointer;

    /// Reference type.
    typedef value_type& reference;

    /// Difference type.
    typedef std::ptrdiff_t difference_type;

    /// Constructs a null-terminated iterator from a pointer.
    ///
    /// @param ptr   A pointer to the first element.
    /// @param end   Whether a past-the-end iterator is to be constructed.
    explicit null_terminated_iterator(pointer ptr, bool end = false)
        : _p(end ? 0 : ptr) {}

    /// Dereferences the iterator.
    reference operator*() const {
        return *_p;
    }

    /// Member access of the element pointed to by the iterator.
    pointer operator->() const {
        return &**this;
    }

    /// Increments the iterator and returns the incremented iterator.
    null_terminated_iterator& operator++() {
        ++_p;
        return *this;
    }

    /// Increments and returns the iterator prior to the increment.
    null_terminated_iterator operator++(int) {
        null_terminated_iterator i = *this;
        ++*this;
        return i;
    }

    /// Compares two iterators for equality.
    bool operator==(const null_terminated_iterator& other) const {
        return _p == other._p || (_end() && other._end());
    }

    /// Compares two iterators for inequality.
    bool operator!=(const null_terminated_iterator& other) const {
        return !(*this == other);
    }

private:
    pointer _p;
    // A null pointer and a pointer to a null character are both valid
    // past-the-end iterators and are treated the same for all intents and
    // purposes.  Because of the presence of this special value, the iterator
    // cannot be bidirectional since that `--end` would not return a valid
    // result.
    //
    // Furthermore, there is no guarantee that i += X will result in defined
    // behavior since the end-point is unknown.  Therefore, operations that
    // increment by more than 1 are unreliable.
    bool _end() const {
        return !_p || *_p == value_type();
    }
};

/// Returns a `ForwardIterator` to the first element of a null-terminated
/// array.
///
/// @param ptr   A pointer to the first element.
///
/// @see null_terminated_iterator
template<class T>
inline null_terminated_iterator<T> null_terminated_begin(T* ptr) {
    return null_terminated_iterator<T>(ptr);
}

/// Returns a past-the-end `ForwardIterator` for a null-terminated array.
///
/// @param ptr   A pointer to the first element.
///
/// @see null_terminated_iterator
template<class T>
inline null_terminated_iterator<T> null_terminated_end(T* ptr) {
    return null_terminated_iterator<T>(ptr, true);
}

}
#endif
