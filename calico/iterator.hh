#ifndef FZ_ITERATOR_HH
#define FZ_ITERATOR_HH
#include <iterator>
#include "cxx11.hh"
namespace fz {
namespace _priv {

// A proxy object that behaves like a pointer.
template<class Ref>
class proxy_pointer {
    Ref _r;
public:
    explicit input_iterator_pointer(Ref& r) _r(r) {}
    Ref* operator->() const { return addressof(_r); }
    Ref operator*() const { return _r; }
    operator Ref*() const { return addressof(_r); }
};

// Constructs from a given reference or proxy-reference type to `T` a pointer
// or proxy-pointer type that has the appropriate semantics.
template<class T, class Ref>
struct reference_to_pointer {
    typedef proxy_pointer<Ref> type;
    type get(Ref& r) { return type(r); }
};
template<class T>
struct reference_to_pointer<T, T&> {
    typedef T* type;
    type get(T& r) { return &r; }
};

}

/// @addtogroup FzIterator Iterators
///
/// Some utility related to iterators.
///
/// @{

/// A base class for implementing input iterators.
///
/// The derived class is required to implement the following operations:
///
/// ~~~{.cpp}
///     bool operator==(const Other&) const;
///     Reference operator*() const;
///     Derived& operator++();
/// ~~~
///
template<class Derived, class T, class Reference = T&>
class input_iterator_base {
    typedef _priv::reference_to_pointer<T, Reference> _pointer;
public:

    /// Difference type.
    typedef std::ptrdiff_t difference_type;

    /// Iterator category.
    typedef std::input_iterator_tag iterator_category;

    /// Reference type.
    typedef Reference reference;

    /// Value type.
    typedef T value_type;

    /// Pointer type.
    typedef typename _pointer::type pointer;

    /// Compares two iterators for inequality.
    template<class Other>
    bool operator!=(const Other& i) const {
        return !(*static_cast<Derived*>(this) == i);
    }

    /// Member access of the element pointed to by the iterator.
    pointer operator->() const {
        return _pointer::get(**static_cast<Derived*>(this));
    }

    /// Increments the iterator and returns the previously pointed-to value.
    Derived& operator++(int) {
        value_type x = **static_cast<Derived*>(this);
        ++*static_cast<Derived*>(this);
        return x;
    }
};

/// An iterator adapter that counts the offset as the iterator moves.
///
/// @tparam Iterator  An iterator type to be wrapped.  The adapter supports
///                   all kinds of iterators.
/// @tparam Distance  The type used to count the iterations.  Defaults to
///                   the `difference_type` of the `Iterator`.
template<class Iterator,
         class Distance =
             typename std::iterator_traits<Iterator>::difference_type>
class counted_iterator {
    typedef std::iterator_traits<Iterator> _traits;
public:

    /// Wrapped iterator type.
    typedef Iterator iterator_type;

    /// Iterator category.
    typedef typename std::iterator_traits<iterator_type>::iterator_category
        iterator_category;

    /// Value type.
    typedef typename std::iterator_traits<iterator_type>::value_type value_type;

    /// Difference type (also used for counting).
    typedef Distance difference_type;

    /// Pointer type.
    typedef typename std::iterator_traits<iterator_type>::pointer pointer;

    /// Reference type.
    typedef typename std::iterator_traits<iterator_type>::reference reference;

    /// Default initializes a `counted_iterator` with unspecified values.
    counted_iterator() {}

    /// Constructs a `counted_iterator` with an initial count.
    ///
    /// @param i            The iterator to be wrapped.
    /// @param init_count   The initial count.
    counted_iterator(iterator_type i,
                     difference_type init_count = difference_type())
        : _i(i), _count(init_count) {}

    template<class OtherIterator>
    counted_iterator(const counted_iterator<OtherIterator>& other)
        : _i(other._i),
          _count(other._count) {}
    template<class OtherIterator>
    counted_iterator& operator=(const counted_iterator<OtherIterator>& other) {
        _i = other._i;
        _count = other._count;
    }

    iterator_type& base() const {
        return _i;
    }
    reference operator*() const {
        return *_i;
    }
    pointer operator->() const {
        return &**this;
    }
    counted_iterator& operator++() {
        ++_i;
        ++_count;
        return *this;
    }
    counted_iterator& operator--() {
        --_i;
        --_count;
        return *this;
    }
    counted_iterator operator++(int) {
        return counted_iterator(_i++, _count++);
    }
    counted_iterator operator--(int) {
        return counted_iterator(_i--, _count--);
    }
    counted_iterator operator+(difference_type n) const {
        return counted_iterator(_i + n, _count + n);
    }
    counted_iterator operator-(difference_type n) const {
        return counted_iterator(_i - n, _count - n);
    }
    counted_iterator& operator+=(difference_type n) const {
        _i += n;
        _count += n;
        return *this;
    }
    counted_iterator& operator-=(difference_type n) const {
        _i -= n;
        _count -= n;
        return *this;
    }
    difference_type count() const {
        return _count;
    }
    void set_count(difference_type value) {
        _count = value;
    }
    template<class OtherIterator>
    bool operator==(const counted_iterator<OtherIterator>& other) const {
        return _i == other._i;
    }
    template<class OtherIterator>
    bool operator!=(const counted_iterator<OtherIterator>& other) const {
        return _i != other._i;
    }
    template<class OtherIterator>
    bool operator<=(const counted_iterator<OtherIterator>& other) const {
        return _i <= other._i;
    }
    template<class OtherIterator>
    bool operator>=(const counted_iterator<OtherIterator>& other) const {
        return _i >= other._i;
    }
    template<class OtherIterator>
    bool operator<(const counted_iterator<OtherIterator>& other) const {
        return _i < other._i;
    }
    template<class OtherIterator>
    bool operator>(const counted_iterator<OtherIterator>& other) const {
        return _i > other._i;
    }
    friend counted_iterator operator+(difference_type n,
                                      const counted_iterator& i) {
        return i + n;
    }
#ifdef HAVE_TRAILING_RETURN
    template<class OtherIterator>
    auto operator-(const counted_iterator<OtherIterator>& i)
      -> decltype(_i - i._i) {
        return _i - i._i;
    }
#else
    difference_type operator-(const counted_iterator& i) {
        return _i - i._i;
    }
#endif
private:
    Iterator _i;
    difference_type _count;
};

/// Constructs a `counted_iterator` with an initial count.
///
/// @param i            The iterator to be wrapped.
/// @param init_count   The initial count.
///
/// @see counted_iterator
template<class Iterator, class Counter>
counted_iterator<Iterator> iterator_counter(
    Iterator i,
    typename counted_iterator<Iterator>::difference_type init_count =
        typename counted_iterator<Iterator>::difference_type()) {
    return counted_iterator<Iterator>(i, init_count);
}

/// @}

}
#endif
