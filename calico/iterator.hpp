#ifndef JWDCQBSMQKBXXFZEIVNU
#define JWDCQBSMQKBXXFZEIVNU
/// @file
///
/// Some utility related to iterators.
///
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include "utility.hpp"
namespace cal {
namespace _priv {

// A proxy object that behaves like a pointer.
template<class Ref>
struct proxy_pointer {
    explicit proxy_pointer(Ref& r) : _r(r) {}
    Ref* operator->() const { return std::addressof(_r); }
    Ref operator*() const { return _r; }
    operator Ref*() const { return std::addressof(_r); }
private:
    Ref _r;
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

/// A base type for implementing input iterators.
///
/// The derived type is should implement the following operations:
///
/// ~~~~cpp
///     bool operator==(const Derived&) const;  // optional
///     Reference operator*() const;            // optional
///     Derived& operator++();
/// ~~~~
///
/// - If `operator==` is not implemented, it defaults to `false` (every
///   iterator is distinct from each other).
/// - If `operator*` is not implemented, it defaults to returning the iterator
///   itself.
///
template<class Derived, class T = Derived, class Reference = T&>
struct input_iterator_base {

    /// Difference type.
    typedef std::ptrdiff_t difference_type;

    /// Iterator category.
    typedef std::input_iterator_tag iterator_category;

    /// Reference type.
    typedef Reference reference;

    /// Value type.
    typedef T value_type;

private:
    typedef _priv::reference_to_pointer<T, Reference> _pointer;
public:

    /// Pointer type.
    typedef CALICO_HIDE(typename _pointer::type) pointer;

    /// Compares two iterators for equality.
    bool operator==(const Derived& i) const { return false; }

    /// Compares two iterators for inequality.
    bool operator!=(const Derived& i) const {
        return !(*static_cast<Derived*>(this) == i);
    }

    /// Returns the pointed-to object.
    reference operator*() const { return *static_cast<Derived*>(this); }

    /// Member access of the object pointed to by the iterator.
    pointer operator->() const {
        return _pointer::get(**static_cast<Derived*>(this));
    }

    /// Increments the iterator and returns the previously pointed-to value.
    Derived& operator++(int) {
        value_type x = **static_cast<Derived*>(this);
        ++*static_cast<Derived*>(this);
        return x;
    }

protected:
    ~input_iterator_base() {}
};

/// An iterator adapter that counts the offset as the iterator moves.
///
/// @tparam Iterator  An iterator type to be wrapped.  The adapter supports
///                   all kinds of iterators.
/// @tparam Distance  The type used to count the iterations.  Defaults to
///                   the `difference_type` of the `Iterator`.
template<
    class Iterator,
    class Distance = typename std::iterator_traits<Iterator>::difference_type
> struct counted_iterator {

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

    /// Default initializes the iterator with an unspecified counter value.
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
    const iterator_type& base() const { return _i; }

    /// Returns the underlying iterator.
    operator iterator_type() const { return _i; }

    /// Returns the current value of the counter.
    difference_type count() const { return _count; }

    /// Sets the current value of the counter.
    void set_count(difference_type value) { _count = value; }

    /// Returns the pointed-to object.
    reference operator*() const { return *_i; }

    /// Member access of the object pointed to by the iterator.
    pointer operator->() const { return &**this; }

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

namespace _priv {
// Can't think of a better way to do this
template<class, class = void> struct difference_type {};
template<class I>             struct difference_type<I,
    typename std::conditional<0, typename I::iterator_category, void>::type
> { typedef typename std::iterator_traits<I>::difference_type type; };
}

// All comparison operators here following the same pattern: 3 overloads with
// (counted, counted), (counted, J), (I, counted) for every operator.
// Verbose, but sadly this is as clean as it gets.

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
CALICO_HIDE(decltype(std::declval<I>() == std::declval<J>()))
operator==(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() == j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() == std::declval<J>()))
operator==(const I& i, const counted_iterator<J, D>& j) {
    return i == j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() == std::declval<J>()))
operator==(const counted_iterator<I, D>& i, const J& j) {
    return i.base() == j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
CALICO_HIDE(decltype(std::declval<I>() != std::declval<J>()))
operator!=(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() != j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() != std::declval<J>()))
operator!=(const I& i, const counted_iterator<J, D>& j) {
    return i != j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() != std::declval<J>()))
operator!=(const counted_iterator<I, D>& i, const J& j) {
    return i.base() != j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
CALICO_HIDE(decltype(std::declval<I>() <= std::declval<J>()))
operator<=(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() <= j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() <= std::declval<J>()))
operator<=(const I& i, const counted_iterator<J, D>& j) {
    return i <= j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() <= std::declval<J>()))
operator<=(const counted_iterator<I, D>& i, const J& j) {
    return i.base() <= j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
CALICO_HIDE(decltype(std::declval<I>() >= std::declval<J>()))
operator>=(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() >= j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() >= std::declval<J>()))
operator>=(const I& i, const counted_iterator<J, D>& j) {
    return i >= j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() >= std::declval<J>()))
operator>=(const counted_iterator<I, D>& i, const J& j) {
    return i.base() >= j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
CALICO_HIDE(decltype(std::declval<I>() < std::declval<J>()))
operator<(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() < j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() < std::declval<J>()))
operator<(const I& i, const counted_iterator<J, D>& j) {
    return i < j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() < std::declval<J>()))
operator<(const counted_iterator<I, D>& i, const J& j) {
    return i.base() < j;
}

/// Compares the underlying iterators.
template<class I, class J, class D, class E> inline
CALICO_HIDE(decltype(std::declval<I>() > std::declval<J>()))
operator>(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() > j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() > std::declval<J>()))
operator>(const I& i, const counted_iterator<J, D>& j) {
    return i > j.base();
}

/// Compares the underlying iterators.
template<class I, class J, class D> inline
CALICO_HIDE(decltype(std::declval<I>() > std::declval<J>()))
operator>(const counted_iterator<I, D>& i, const J& j) {
    return i.base() > j;
}

// ... and we're finally done with all the comparison operators.

/// Returns an iterator in which the underlying iterator and the counter
/// are both advanced by `n`.
template<class I, class D, class N> inline
CALICO_HIDE(decltype(
    counted_iterator<I, D>(
        std::declval<I>() + std::declval<N>(),
        std::declval<D>() + std::declval<N>()
    )
)) operator+(const counted_iterator<I, D>& i, N n) {
    return counted_iterator<I, D>(i.base() + n, i.count()+ n);
}

/// Returns an iterator advanced by the given number of steps.
template<class I, class D, class N> inline
CALICO_HIDE(decltype(std::declval<I>() + std::declval<N>()))
operator+(N n, const counted_iterator<I, D>& i) {
    return i + n;
}

/// Returns an iterator in which the underlying iterator and the counter
/// are both advanced by `n` in reverse.
template<class I, class D, class N> inline
CALICO_HIDE(decltype(
    counted_iterator<I, D>(
        std::declval<I>() - std::declval<N>(),
        std::declval<D>() - std::declval<N>()
    )
)) operator-(const counted_iterator<I, D>& i, N n) {
    return counted_iterator<I, D>(i.base() - n, i.count() - n);
}

/// Returns the distance between two iterators.
template<class I, class J, class D, class E> inline
CALICO_HIDE(decltype(std::declval<I>() - std::declval<J>()))
operator-(const counted_iterator<I, D>& i, const counted_iterator<J, E>& j) {
    return i.base() - j.base();
}

/// Returns the distance between two iterators.
template<class I, class J, class D> inline
CALICO_HIDE((typename std::conditional<0,
    typename _priv::difference_type<I>::type,
    decltype(std::declval<I>() - std::declval<J>())
>::type)) operator-(const I& i, const counted_iterator<J, D>& j) {
    return i - j.base();
}

/// Returns the distance between two iterators.
template<class I, class J, class D> inline
CALICO_HIDE((typename std::conditional<0,
    typename _priv::difference_type<J>::type,
    decltype(std::declval<I>() - std::declval<J>())
>::type)) operator-(const counted_iterator<I, D>& i, const J& j) {
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

/// An `RandomAccessIterator` that stores an integer value of type `T`.  The
/// dereferenced value of the iterator is the integer itself.
///
/// @tparam T     An integer type to be wrapped.
/// @tparam Tag   An arbitrary tag used to distinguish between specializations
///               of this template.
template<class T, class Tag = void>
struct integer_iterator {

    /// Value type.
    typedef T value_type;

    /// Difference_type.
    typedef value_type difference_type;

    /// Pointer type.
    typedef const value_type* pointer;

    /// Reference type.
    typedef const value_type& reference;

    /// Iterator category.
    typedef std::random_access_iterator_tag iterator_category;

    /// Constructs an iterator with the given value.
    explicit integer_iterator(value_type i = value_type()) : _i(i) {}

    /// Returns the integer value of the iterator.
    reference operator*() const { return _i; }

    /// Member access of the object pointed to by the iterator.
    pointer operator->() const { return &*this; }

    /// Returns an iterator with value equal to the current value plus an
    /// integer.
    reference operator[](const difference_type& n) const {
        return *(*this + n);
    }

    /// Compares the value of the iterators.
    bool operator==(const integer_iterator& other) const {
        return _i == other._i;
    }

    /// Compares the value of the iterators.
    bool operator!=(const integer_iterator& other) const {
        return _i != other._i;
    }

    /// Compares the value of the iterators.
    bool operator<=(const integer_iterator& other) const {
        return _i <= other._i;
    }

    /// Compares the value of the iterators.
    bool operator>=(const integer_iterator& other) const {
        return _i >= other._i;
    }

    /// Compares the value of the iterators.
    bool operator<(const integer_iterator& other) const {
        return _i < other._i;
    }

    /// Compares the value of the iterators.
    bool operator>(const integer_iterator& other) const {
        return _i > other._i;
    }

    /// Pre-increments the value of the iterator.
    integer_iterator& operator++() { ++_i; return *this; }

    /// Post-increments the value of the iterator.
    integer_iterator operator++(int) { return integer_iterator(_i++); }

    /// Increases the value of the iterator.
    integer_iterator& operator+=(const difference_type& n) {
        _i += n;
        return *this;
    }

    /// Pre-decrements the value of the iterator.
    integer_iterator& operator--() { --_i; return *this; }

    /// Post-decrements the value of the iterator.
    integer_iterator operator--(int) { return integer_iterator(_i--); }

    /// Decreases the value of the iterator.
    integer_iterator& operator-=(const difference_type& n) {
        _i -= n;
        return *this;
    }

    /// Adds an integer to the value of the iterator.
    friend integer_iterator
    operator+(const integer_iterator& i, const difference_type& j) {
        return integer_iterator(i._i + j);
    }

    /// Adds an integer to the value of the iterator.
    friend integer_iterator
    operator+(const difference_type& i, const integer_iterator& j) {
        return j + i;
    }

    /// Subtracts an integer from the value of the iterator.
    friend integer_iterator
    operator-(const integer_iterator& i, const difference_type& j) {
        return integer_iterator(i._i - j);
    }

    /// Subtracts an integer from the value of the iterator.
    friend integer_iterator
    operator-(const difference_type& i, const integer_iterator& j) {
        return j - i;
    }

    /// Subtracts the values of the two iterators.
    friend difference_type
    operator-(const integer_iterator& i, const integer_iterator& j) {
        return i._i - j._i;
    }

    /// Returns an iterator with value equal to `T()`.
    template<class Container> static
    integer_iterator begin(const Container&) {
        return integer_iterator(value_type());
    }

    /// Returns an iterator with value equal to the `size()` of the container.
    ///
    /// Requirements:
    /// - `FiniteContainer` has a method named `size()`.
    template<class Container> static
    integer_iterator end(const Container& c) {
        return integer_iterator(static_cast<value_type>(c.size()));
    }

private:
    value_type _i;
};

/// Base class for defining an immutable, iterable container.
///
/// A minimal definition of this container requires overriding the `begin()`,
/// and `end()` methods in the `Derived` class.  If none are defined, the base
/// class will call the *static* functions `begin(Derived)` and `end(Derived)`
/// defined in the iterator.
template<
    class Derived,
    class Iterator,
    class Size =
    CALICO_HIDE(
        typename std::make_unsigned<
            typename std::iterator_traits<Iterator>::difference_type
        >::type
    )
>
struct const_container_base {

    /// Size type.
    typedef Size size_type;

    /// Const iterator.
    typedef Iterator const_iterator;

    /// Const pointer.
    typedef CALICO_HIDE(
        typename std::iterator_traits<const_iterator>::pointer
    ) const_pointer;

    /// Const reference.
    typedef CALICO_HIDE(
        typename std::iterator_traits<const_iterator>::referenc
    ) const_reference;

    /// Difference type.
    typedef CALICO_HIDE(
            typename std::iterator_traits<const_iterator>::difference_type
    ) difference_type;

    /// Value type.
    typedef CALICO_HIDE(
        typename std::iterator_traits<const_iterator>::value_type
    ) value_type;

    /// Creates a container.
    explicit const_container_base() {}

    /// Returns whether the container is empty.
    bool empty() const {
        const Derived* dthis = static_cast<const Derived*>(this);
        return dthis->size() == size_type();
    }

    /// Returns the number of elements in the container.
    size_type size() const {
        using std::distance;
        const Derived* dthis = static_cast<const Derived*>(this);
        return static_cast<size_type>(distance(dthis->begin(), dthis->end()));
    }

    /// Returns a reference to the first element in the container (provided
    /// that the container is not empty).
    const_reference front() const {
        const Derived* dthis = static_cast<const Derived*>(this);
        return *dthis->cbegin();
    }

    /// Returns a reference to the last element in the container (provided
    /// that the container is not empty and the container is bidirectional).
    CALICO_ENABLE_IF(
        (std::is_base_of<
             std::bidirectional_iterator_tag,
             typename std::iterator_traits<const_iterator>::iterator_category
         >::value),
        const_reference
    ) back() const {
        const Derived* dthis = static_cast<const Derived*>(this);
        return *--dthis->cend();
    }

    /// Returns a const-qualified iterator to the beginning of the container.
    const_iterator begin() const {
        const Derived* dthis = static_cast<const Derived*>(this);
        return const_iterator::begin(*dthis);
    }

    /// Returns a const-qualified iterator to the beginning of the container.
    const_iterator cbegin() const {
        const Derived* dthis = static_cast<const Derived*>(this);
        return dthis->begin();
    }

    /// Returns a const-qualified iterator to the end of the container.
    const_iterator end() const {
        const Derived* dthis = static_cast<const Derived*>(this);
        return const_iterator::end(*dthis);
    }

    /// Returns a const-qualified iterator to the end of the container.
    const_iterator cend() const {
        const Derived* dthis = static_cast<const Derived*>(this);
        return dthis->end();
    }

    /// Accesses the element at a given index (if the container supports
    /// random access).
    CALICO_ENABLE_IF(
        (std::is_base_of<
             std::random_access_iterator_tag,
             typename std::iterator_traits<const_iterator>::iterator_category
         >::value),
        const_reference
    ) at(size_type index) const {
        // First check is not strictly needed, but can be useful in case
        // `size_type` happens to be a signed type.
        const Derived* dthis = static_cast<const Derived*>(this);
        if (index < size_type() || index >= dthis->size())
            throw std::out_of_range("index out of range");
        return (*dthis)[index];
    }

    /// Accesses the element at a given index (if the container supports
    /// random access).
    CALICO_ENABLE_IF(
        (std::is_base_of<
             std::random_access_iterator_tag,
             typename std::iterator_traits<const_iterator>::iterator_category
         >::value),
        const_reference
    ) operator[](size_type index) const {
        const Derived* dthis = static_cast<const Derived*>(this);
        return *(dthis->cbegin() + index);
    }

protected:
    ~const_container_base() {};
};

template<class InputIterator>
struct iterator_range
    : const_container_base<iterator_range<InputIterator>, InputIterator> {
    typedef InputIterator iterator_type;
    iterator_range(const iterator_type& first, const iterator_type& last)
        : first(first), last(last) {}
    iterator_type first;
    iterator_type last;
    iterator_type begin() const { return first; }
    iterator_type end()   const { return last;  }
};

template<class InputIterator> inline
iterator_range<InputIterator>
make_range(const InputIterator& first, const InputIterator& last) {
    return iterator_range<InputIterator>(first, last);
}

/// Constructs an iterator range of integers from `T()` to `end`.
template<class T> inline
iterator_range<integer_iterator<T> >
integer_range(const T& end) {
    return iterator_range<integer_iterator<T> >(
        integer_iterator<T>(),
        integer_iterator<T>(end)
    );
}

/// Constructs an iterator range of integers from `begin` to `end`.
template<class T> inline
iterator_range<integer_iterator<T> >
integer_range(const T& begin, const T& end) {
    return iterator_range<integer_iterator<T> >(
        integer_iterator<T>(begin),
        integer_iterator<T>(end)
    );
}

/// An iterator that applies a function to each element.
template<class InputIterator, class UnaryOperation>
struct transform_iterator {

    /// Underlying iterator type.
    typedef InputIterator iterator_type;

    /// Iterator category.
    typedef CALICO_HIDE(
        typename std::iterator_traits<iterator_type>::iterator_category
    ) iterator_category;

    /// Difference type.
    typedef CALICO_HIDE(
        typename std::iterator_traits<iterator_type>::difference_type
    ) difference_type;

    /// Value type.
    typedef CALICO_HIDE(
        typename std::result_of<
            UnaryOperation(
                typename std::iterator_traits<iterator_type>::reference
            )
        >::type
    ) value_type;

    /// Reference type.
    typedef value_type reference;

private:
    typedef _priv::reference_to_pointer<value_type, reference> _pointer;

public:
    /// Pointer type.
    typedef CALICO_HIDE(typename _pointer::type) pointer;

    /// Default initializer.
    transform_iterator() {}

    /// Constructs an iterator that applies a function to each element.
    transform_iterator(
        const iterator_type& it,
        const UnaryOperation& op
    ) : _it(it), _op(op) {}

    /// Returns the beginning iterator.
    ///
    /// Requirements:
    /// - Underlying iterator must support `begin()`.
    /// - `UnaryOperation` must be default-initializable.
    template<class Container> static
    CALICO_VALID_TYPE(
        decltype(iterator_type::begin(std::declval<Container>())),
        transform_iterator
    ) begin(const Container& c) {
        return transform_iterator(iterator_type::begin(c), UnaryOperation());
    }

    /// Returns the past-the-end iterator.
    ///
    /// Requirements:
    /// - Underlying iterator must support `end()`.
    /// - `UnaryOperation` must be default-initializable.
    template<class Container> static
    CALICO_VALID_TYPE(
        decltype(iterator_type::end(std::declval<Container>())),
        transform_iterator
    ) end(const Container& c) {
        return transform_iterator(iterator_type::end(c), UnaryOperation());
    }

    /// Returns the underlying iterator.
    const iterator_type& base() const { return _it; }

    /// Returns the function object.
    const UnaryOperation& function() const { return _op; }

    /// Returns the pointed-to object.
    reference operator*() const { return _op(*_it); }

    /// Member access of the object pointed to by the iterator.
    pointer operator->() const { return _pointer::get(_op(*_it)); }

    /// Pre-increments the iterator.
    transform_iterator& operator++() {
        ++_it;
        return *this;
    }

    /// Post-increments the iterator.
    transform_iterator operator++(int) {
        transform_iterator t = *this;
        ++*this;
        return t;
    }

    /// Pre-decrements the iterator.
    CALICO_VALID_TYPE(
        decltype(--std::declval<iterator_type>()),
        transform_iterator&
    ) operator--() {
        --_it;
        return *this;
    }

    /// Post-decrements the iterator.
    CALICO_VALID_TYPE(
        decltype(std::declval<iterator_type>()--),
        transform_iterator
    ) operator--(int) {
        transform_iterator t = *this;
        --*this;
        return t;
    }

    /// Advances the iterator by `n`.
    CALICO_VALID_TYPE(
        decltype(std::declval<iterator_type>() +=
                 std::declval<difference_type>()),
        transform_iterator&
    ) operator+=(difference_type n) {
        _it += n;
        return *this;
    }

    /// Advances the iterator by `n` in reverse.
    CALICO_VALID_TYPE(
        decltype(std::declval<iterator_type>() -=
                 std::declval<difference_type>()),
        transform_iterator&
    ) operator-=(difference_type n) {
        _it -= n;
        return *this;
    }

private:
    iterator_type _it;
    UnaryOperation _op;
};

/// Compares the underlying iterators.
template<class I, class F> inline
CALICO_HIDE(decltype(std::declval<I>() == std::declval<I>()))
operator==(const transform_iterator<I, F>& i,
           const transform_iterator<I, F>& j) {
    return i.base() == j.base();
}

/// Compares the underlying iterators.
template<class I, class F> inline
CALICO_HIDE(decltype(std::declval<I>() != std::declval<I>()))
operator!=(const transform_iterator<I, F>& i,
           const transform_iterator<I, F>& j) {
    return i.base() != j.base();
}

/// Compares the underlying iterators.
template<class I, class F> inline
CALICO_HIDE(decltype(std::declval<I>() <= std::declval<I>()))
operator<=(const transform_iterator<I, F>& i,
           const transform_iterator<I, F>& j) {
    return i.base() <= j.base();
}

/// Compares the underlying iterators.
template<class I, class F> inline
CALICO_HIDE(decltype(std::declval<I>() >= std::declval<I>()))
operator>=(const transform_iterator<I, F>& i,
           const transform_iterator<I, F>& j) {
    return i.base() >= j.base();
}

/// Compares the underlying iterators.
template<class I, class F> inline
CALICO_HIDE(decltype(std::declval<I>() < std::declval<I>()))
operator<(const transform_iterator<I, F>& i,
          const transform_iterator<I, F>& j) {
    return i.base() < j.base();
}

/// Compares the underlying iterators.
template<class I, class F> inline
CALICO_HIDE(decltype(std::declval<I>() > std::declval<I>()))
operator>(const transform_iterator<I, F>& i,
          const transform_iterator<I, F>& j) {
    return i.base() > j.base();
}

/// Returns an iterator advanced by `n`.
template<class I, class F> inline
CALICO_HIDE(decltype(
    transform_iterator<I, F>(
        std::declval<I>() +
        std::declval<typename transform_iterator<I, F>::difference_type>(),
        std::declval<F>())
)) operator+(const transform_iterator<I, F>& i,
            typename transform_iterator<I, F>::difference_type n) {
    return transform_iterator<I, F>(i.base() + n, i.function());
}

/// Returns an iterator advanced by `n`.
template<class I, class F> inline
CALICO_HIDE(decltype(
    std::declval<transform_iterator<I, F> >() +
    std::declval<typename transform_iterator<I, F>::difference_type>()
)) operator+(typename transform_iterator<I, F>::difference_type n,
            const transform_iterator<I, F>& i) { return i + n; }

/// Returns an iterator advanced by `n` in reverse.
template<class I, class F> inline
CALICO_HIDE(decltype(
    transform_iterator<I, F>(
        std::declval<I>() -
        std::declval<typename transform_iterator<I, F>::difference_type>(),
        std::declval<F>())
)) operator-(const transform_iterator<I, F>& i,
          typename transform_iterator<I, F>::difference_type n) {
    return transform_iterator<I, F>(i.base() - n, i.function());
}

/// Returns the distance between two iterators.
template<class I, class F> inline
CALICO_HIDE(decltype(std::declval<I>() - std::declval<I>()))
operator-(const transform_iterator<I, F>& i,
            const transform_iterator<I, F>& j) {
    return i.base() - j.base();
}

/// A iterable container that represents the results from a `transform`
/// function call.
template<class InputIterator, class UnaryOperation>
struct transformed_range
    : const_container_base<
          transformed_range<InputIterator, UnaryOperation>,
          transform_iterator<InputIterator, UnaryOperation> > {

    /// Const iterator type.
    typedef transform_iterator<InputIterator, UnaryOperation> const_iterator;

    /// Default constructor.
    transformed_range() {}

    /// Constructor.
    transformed_range(
        const InputIterator& first,
        const InputIterator& last,
        const UnaryOperation& op
    ) : _first(first), _last(last), _op(op) {}

    /// Returns the beginning to the container.
    const_iterator begin() const { return const_iterator(_first, _op); }

    /// Returns the end to the container.
    const_iterator end()   const { return const_iterator(_last,  _op); }

private:
    InputIterator _first;
    InputIterator _last;
    UnaryOperation _op;
};

/// Applies a given function to every element of an iterator range and returns
/// the result as a lazily evaluated iterable container (i.e. the "map"
/// function).
template<class InputIterator, class UnaryOperation> inline
CALICO_ALT((
    transformed_range<InputIterator, UnaryOperation>
), transformed_range<..>) transform(
    const InputIterator& first,
    const InputIterator& last,
    const UnaryOperation& op
) {
    return transformed_range<InputIterator, UnaryOperation>(first, last, op);
}

/// Applies a given function to every element of an iterable container and
/// returns the result as a lazily evaluated iterable container (i.e. the
/// "map" function).
template<class Container, class UnaryOperation> inline
CALICO_ALT((transformed_range<
    decltype(begin(std::declval<const Container&>())),
    UnaryOperation
>), transformed_range<..>) transform(
    const Container& c,
    const UnaryOperation& op
) {
    return transformed_range<
               decltype(begin(std::declval<const Container&>())),
               UnaryOperation
           >(begin(c), end(c), op);
}

}
#endif
