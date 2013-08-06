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
    explicit proxy_pointer(Ref& r) : _r(r) {}
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

template<class, class>
class counted_iterator;
namespace _priv {
struct counted_iterator_ {
    // Can't think of a better way to do this
    template<class, class = void> struct difference_type {};
    template<class I>             struct difference_type<I,
        FZ_VALID_TYPE(typename I::iterator_category, void)
    > { typedef typename std::iterator_traits<I>::difference_type type; };
    };
}

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

    /// Underlying iterator type.
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

    /// Default initializes a `counted_iterator` with an unspecified counter
    /// value.
    counted_iterator() {}

    /// Constructs a `counted_iterator` with an initial count.
    ///
    /// @param iterator     The iterator to be wrapped.
    /// @param init_count   The initial count.
    counted_iterator(
        const iterator_type& iterator,
        difference_type init_count = difference_type()
    ) : _i(iterator), _count(init_count) {}

    /// Returns the underlying iterator.
    const iterator_type& base() const {
        return _i;
    }

    /// Returns the underlying iterator.
    operator iterator_type() const {
        return _i;
    }

    /// Returns the current value of the counter.
    difference_type count() const {
        return _count;
    }

    /// Sets the current value of the counter.
    void set_count(difference_type value) {
        _count = value;
    }

    /// Returns the pointed-to object.
    reference operator*() const {
        return *_i;
    }

    /// Allows member access for the pointed-to object.
    pointer operator->() const {
        return &**this;
    }

    /// Post-increments the underlying iterator and the counter.
    counted_iterator operator++(int) {
        return counted_iterator(_i++, _count++);
    }

    /// Pre-increments the underlying iterator and the counter.
    counted_iterator& operator++() {
        ++_i;
        ++_count;
        return *this;
    }

    /// Pre-decrements the underlying iterator and the counter.
    counted_iterator& operator--() {
        --_i;
        --_count;
        return *this;
    }

    /// Post-decrements the underlying iterator and the counter.
    counted_iterator operator--(int) {
        return counted_iterator(_i--, _count--);
    }

    /// Advances both the underlying iterator and the counter by `n`.
    template<class N>
    counted_iterator& operator+=(N n) {
        _i += n;
        _count += n;
        return *this;
    }

    /// Advances both the underlying iterator and the counter by `n` in
    /// reverse.
    template<class N>
    counted_iterator& operator-=(N n) {
        _i -= n;
        _count -= n;
        return *this;
    }

private:
    iterator_type _i;
    difference_type _count;
};

// All comparison operators here following the same pattern: 3 overloads with
// (counted, counted), (counted, J), (I, counted) for every operator.
// Verbose, but sadly this is as clean as it gets.

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
FZ_DECLTYPE(declval<I>() == declval<J>(), bool)
operator==(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() == j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() == declval<J>(), bool)
operator==(const I& i, const counted_iterator<J, D>& j) {
    return i == j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() == declval<J>(), bool)
operator==(const counted_iterator<I, D>& i, const J& j) {
    return i.base() == j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
FZ_DECLTYPE(declval<I>() != declval<J>(), bool)
operator!=(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() != j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() != declval<J>(), bool)
operator!=(const I& i, const counted_iterator<J, D>& j) {
    return i != j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() != declval<J>(), bool)
operator!=(const counted_iterator<I, D>& i, const J& j) {
    return i.base() != j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
FZ_DECLTYPE(declval<I>() <= declval<J>(), bool)
operator<=(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() <= j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() <= declval<J>(), bool)
operator<=(const I& i, const counted_iterator<J, D>& j) {
    return i <= j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() <= declval<J>(), bool)
operator<=(const counted_iterator<I, D>& i, const J& j) {
    return i.base() <= j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
FZ_DECLTYPE(declval<I>() >= declval<J>(), bool)
operator>=(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() >= j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() >= declval<J>(), bool)
operator>=(const I& i, const counted_iterator<J, D>& j) {
    return i >= j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() >= declval<J>(), bool)
operator>=(const counted_iterator<I, D>& i, const J& j) {
    return i.base() >= j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
FZ_DECLTYPE(declval<I>() < declval<J>(), bool)
operator<(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() < j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() < declval<J>(), bool)
operator<(const I& i, const counted_iterator<J, D>& j) {
    return i < j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() < declval<J>(), bool)
operator<(const counted_iterator<I, D>& i, const J& j) {
    return i.base() < j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
FZ_DECLTYPE(declval<I>() > declval<J>(), bool)
operator>(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() > j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() > declval<J>(), bool)
operator>(const I& i, const counted_iterator<J, D>& j) {
    return i > j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(declval<I>() > declval<J>(), bool)
operator>(const counted_iterator<I, D>& i, const J& j) {
    return i.base() > j;
}

// ... and we're finally done with all the comparison operators.

/// Returns an iterator in which the underlying iterator and the counter
/// are both advanced by `n`.
template<class I, class D, class N> inline
FZ_DECLTYPE(
    (counted_iterator<I, D>(
        declval<I>() + declval<N>(),
        declval<D>() + declval<N>()
    )),
    (counted_iterator<I, D>)
) operator+(const counted_iterator<I, D>& i, N n) {
    return counted_iterator<I, D>(i.base() + n, i.count()+ n);
}

/// Returns an iterator advanced by the given number of steps.
template<class I, class D, class N> inline
FZ_DECLTYPE(declval<I>() + declval<N>(), (counted_iterator<I, D>))
operator+(N n, const counted_iterator<I, D>& i) {
    return i + n;
}

/// Returns an iterator in which the underlying iterator and the counter
/// are both advanced by `n` in reverse.
template<class I, class D, class N> inline
FZ_DECLTYPE(
    (counted_iterator<I, D>(
        declval<I>() - declval<N>(),
        declval<D>() - declval<N>()
    )),
    (counted_iterator<I, D>)
) operator-(const counted_iterator<I, D>& i, N n) {
    return counted_iterator<I, D>(i.base() - n, i.count() - n);
}

/// Returns the distance between two iterators.
template<class I, class J, class D, class E> inline
FZ_DECLTYPE(declval<I>() - declval<J>(), (typename common_type<D, E>::type))
operator-(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() - j.base();
}

/// Returns the distance between two iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(
    declval<I>() - declval<J>(),
    (typename common_type<
         typename _priv::counted_iterator_::difference_type<J>::type,
         D
     >::type)
) operator-(const I& i, const counted_iterator<J, D>& j) {
    return i - j.base();
}

/// Returns the distance between two iterators.
template<class I, class J, class D> inline
FZ_DECLTYPE(
    declval<I>() - declval<J>(),
    (typename common_type<
         D,
         typename _priv::counted_iterator_::difference_type<J>::type
     >::type)
) operator-(const counted_iterator<I, D>& i, const J& j) {
    return i.base() - j;
}

/// Constructs a `counted_iterator` with an initial count.
///
/// @param iterator     The iterator to be wrapped.
/// @param init_count   The initial count.
///
/// @see counted_iterator
template<class Iterator> inline
counted_iterator<Iterator> iterator_counter(
    const Iterator& iterator,
    typename counted_iterator<Iterator>::difference_type init_count =
        typename counted_iterator<Iterator>::difference_type()
) { return counted_iterator<Iterator>(iterator, init_count); }

/// @}

}
#endif
