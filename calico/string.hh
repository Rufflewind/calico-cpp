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

/// A `RandomAccessIterator` that allows traversing the elements of a
/// null-terminated string.  Note that traversing beyond the range of the
/// string or dereferencing the last iterator is undefined behavior.
template<class CharT>
class null_terminated_iterator
    : public std::iterator<std::random_access_iterator_tag, CharT> {
public:
    typedef std::iterator<std::random_access_iterator_tag, CharT> base_type;
    typedef null_terminated_iterator this_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::reference reference;
    typedef typename base_type::difference_type difference_type;
    explicit null_terminated_iterator(pointer s = 0) : _p(s) {}
    reference operator*() const {
        return *_p;
    }
    pointer operator->() const {
        return &**this;
    }
    this_type& operator++() {
        return *this += 1;
    }
    this_type operator++(int) {
        this_type i = *this;
        ++*this;
        return i;
    }
    this_type& operator+=(difference_type offset) {
        _p += offset;
        return *this;
    }
    this_type& operator-=(difference_type offset) {
        return *this += -offset;
    }
    bool operator==(const this_type& other) const {
        return _p == other._p || (_end() && other._end());
    }
    bool operator!=(const this_type& other) const {
        return !(*this == other);
    }
    bool operator<=(const this_type& other) const {
        return other._end() || (!_end() && _p <= other._p);
    }
    bool operator<(const this_type& other) const {
        return *this <= other && *this != other;
    }
    bool operator>=(const this_type& other) const {
        return !(*this < other);
    }
    bool operator>(const this_type& other) const {
        return !(*this <= other);
    }
    this_type operator+(difference_type n) const {
        this_type self = *this;
        return self += n;
    }
    this_type operator-(difference_type n) const {
        return *this + (-n);
    }
    difference_type operator-(const this_type& i) const {
        return _p - i._p;
    }
    reference operator[](const difference_type& n) const {
        return *(*this + n);
    }
    friend this_type operator+(difference_type n, const this_type& i) {
        return i + n;
    }
    friend this_type operator-(difference_type n, const this_type& i) {
        return i - n;
    }
private:
    pointer _p;
    bool _end() const {
        // A null pointer and a pointer to a null character are both valid end
        // iterators and are treated the same for all intents and purposes.
        // Because of the presence of this special value, the iterator cannot
        // be truly Bidirectional since that `--end_iterator` would not return
        // a valid result.
        return !_p || *_p == value_type();
    }
};

/// Returns an iterator to the beginning of a null-terminated string.
template<class CharT>
inline null_terminated_iterator<CharT> begin_cstr(CharT* s) {
    return null_terminated_iterator<CharT>(s);
}

/// Returns an iterator to the end a null-terminated string with O(1)
/// complexity.
template<class CharT>
inline null_terminated_iterator<CharT> end_cstr(CharT*) {
    // Return the universal end iterator so we don't have to compute 'strlen'
    return null_terminated_iterator<CharT>();
}

}
#endif
