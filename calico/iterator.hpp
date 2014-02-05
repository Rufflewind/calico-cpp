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

// Implements ADL lookup for `begin` and `end`.

using std::begin;
using std::end;

template<class Container> inline
auto adl_begin(const Container& c)
-> decltype(begin(c))
{   return  begin(c); }

template<class Container> inline
auto adl_end(const Container& c)
-> decltype(end(c))
{   return  end(c); }

// Requires `rbegin` and `rend` as member functions.  This may be relaxed in
// the future when C++14 support is improved.

template<class Container> inline
auto adl_rbegin(const Container& c)
-> decltype(c.rbegin())
{   return  c.rend(); }

template<class Container> inline
auto adl_rend(const Container& c)
-> decltype(c.rend())
{   return  c.rend(); }

}

/// CRTP base type for implementing input iterators.
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
    typedef
#ifdef CALICO_DOC_ONLY
        auto
#else
        typename _pointer::type
#endif
        pointer;

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
    ~input_iterator_base() {} // don't try to initialize a CRTP base type
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

private:
    value_type _i;
};

/// CRTP base type for defining an mutable, iterable container.
///
/// @tparam Derived        The derived type.
/// @tparam ConstIterator  An iterator type for read-only access.
/// @tparam Iterator       An iterator type that is convertible to
///                        `ConstIterator`.  If the container is immutable,
///                        it must be the same as `ConstIterator`.
/// @tparam Size           The size type.  Defaults to the `common_type` of the
///                        unsigned `difference_type`s of the iterators.
///
/// The `Derived` type must override certain methods in order for this base
/// type to work correctly.  It must provide each of the following at minimum:
///
/// - Either one of the following (starting with the lowest precedence):
///   - `data` (equivalent to `_begin`)
///   - `_begin`
///   - `begin const` and `begin` (the latter is optional if the container
///     is immutable).
/// - Either one of the following (starting with the lowest precedence):
///   - `size`
///   - `_end`
///   - `end const` and `end` (the latter is optional if the container
///     is immutable).
///
/// The listing below shows the dependencies of each function defined by the
/// base type.  The arrow (`<==`) may be read as "depends on".  Any of these
/// functions may be overridden to customize the behavior of the container.
///
/// **Note:** Keep in mind that if the `Derived` type defines a function, the
/// name of the function becomes shadowed so all overloads of this function
/// provided by the base type become unavailable unless explicitly imported
/// via a `using` declaration.
///
/// ~~~~cpp
///
///  size const <==  begin const && end const
/// _begin      <==  data /* not defined */
/// _end        <==  begin && size
///  begin      <== _begin
///  end        <== _end
///
/// // special cases for consts
///  begin const <== begin
///  end   const <== end
/// cbegin const <== begin const
/// cend   const <== end   const
///
/// // queries
/// empty    <== begin const && end const
/// [] const <== begin const
/// []       <== begin
/// at const <== [] const
/// at       <== []
///
/// // front & back
/// front const <== begin const
/// front       <== begin
/// back  const <== end const
/// back        <== end
///
/// // reverse iterators
///  rbegin const <==  end   const
///  rend   const <==  begin const
///  rbegin       <==  end
///  rend         <==  begin
/// crbegin       <== rbegin const
/// crend         <== rend   const
///
/// ~~~~
///
template<
    class Derived,
    class ConstIterator,
    class Iterator = ConstIterator,
    class Size =
    CALICO_HIDE(
        (typename std::common_type<
             typename std::make_unsigned<
                 typename std::iterator_traits<ConstIterator>::difference_type
             >::type,
             typename std::make_unsigned<
                 typename std::iterator_traits<Iterator>::difference_type
             >::type
         >::type)
    )
>
struct container_base {

    /// Size type.
    typedef Size size_type;

    /// Const iterator type.
    typedef ConstIterator const_iterator;

    /// Iterator type.
    typedef Iterator iterator;

    /// Difference type.
    typedef typename std::common_type<
              typename std::iterator_traits<const_iterator>::difference_type,
              typename std::iterator_traits<iterator>::difference_type
           >::type
        difference_type;

    /// Value type.
    typedef typename std::common_type<
              typename std::iterator_traits<const_iterator>::value_type,
              typename std::iterator_traits<iterator>::value_type
           >::type
        value_type;

    /// Const reference type.
    typedef typename std::iterator_traits<const_iterator>::reference
        const_reference;

    /// Reference type.
    typedef typename std::iterator_traits<iterator>::reference
        reference;

    /// Const pointer type.
    typedef typename std::iterator_traits<const_iterator>::pointer
        const_pointer;

    /// Pointer type.
    typedef typename std::iterator_traits<iterator>::pointer
        pointer;

    /// Const reverse iterator.
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    /// Reverse iterator.
    typedef std::reverse_iterator<iterator> reverse_iterator;

    /// Returns whether the container is empty.
    ///
    /// Depends on `begin() const` and `end() const`.
    bool empty() const {
        const Derived& dthis = static_cast<const Derived&>(*this);
        return dthis.begin() == dthis.end();
    }

    /// Returns the number of elements in the container.
    ///
    /// Depends on `begin() const` and `end() const` and calls
    /// `std::distance()` with ADL.
    size_type size() const {
        using std::distance;
        const Derived& dthis = static_cast<const Derived&>(*this);
        return static_cast<size_type>(distance(dthis.begin(), dthis.end()));
    }

    /// Returns a `const_reference` to the first element in the container
    ///
    /// Depends on `begin() const`.  If the container is empty, the result is
    /// undefined.
    const_reference front() const {
        return static_cast<const Derived&>(*this).begin();
    }

    /// Returns a `reference` to the first element in the container.
    ///
    /// Depends on `begin()`.  If the container is empty, the result is
    /// undefined.
    reference front() {
        return static_cast<Derived&>(*this).begin();
    }

    /// Returns a `const_reference` to the last element in the container.
    ///
    /// Depends on `end() const` and is only defined if the iterator is
    /// bidirectional.  If the container is empty, the result is undefined.
    const_reference back() const {
        return *--static_cast<const Derived&>(*this).end();
    }

    /// Returns a `reference` to the last element in the container.
    ///
    /// Depends on `end()` and is only defined if the iterator is
    /// bidirectional.  If the container is empty, the result is undefined.
    reference back() {
        return *--static_cast<Derived&>(*this).end();
    }

    /// Returns an `iterator` to the beginning of the container.
    ///
    /// Depends on `_begin()`.
    iterator begin() {
        return static_cast<Derived&>(*this)._begin();
    }

    /// Returns an `iterator` to the end of the container.
    ///
    /// Depends on `_end()`.
    iterator end() {
        return static_cast<Derived&>(*this)._end();
    }

    /// Returns a `const_iterator` to the beginning of the container.
    ///
    /// Depends on the non-const `begin()` and uses a `const_cast`.
    const_iterator begin() const {
        return static_cast<const_iterator>(
            const_cast<Derived&>(
                static_cast<const Derived&>(*this)).begin());
    }

    /// Returns a `const_iterator` to the end of the container.
    ///
    /// Depends on the non-const `end()` and uses a `const_cast`.
    const_iterator end() const {
        return static_cast<const_iterator>(
            const_cast<Derived&>(
                static_cast<const Derived&>(*this)).end());
    }

    /// Returns a `const_iterator` to the beginning of the container.
    ///
    /// Depends on `begin() const`.
    const_iterator cbegin() const {
        return static_cast<const Derived&>(*this).begin();
    }

    /// Returns a `const_iterator` to the end of the container.
    ///
    /// Depends on `end() const`.
    const_iterator cend() const {
        return static_cast<const Derived&>(*this).end();
    }

    /// Returns a `const_reverse_iterator` to the beginning of the container.
    ///
    /// Depends on `end()`.
    reverse_iterator rbegin() {
        return reverse_iterator(static_cast<Derived&>(*this).end());
    }

    /// Returns a `const_reverse_iterator` to the end of the container.
    ///
    /// Depends on `begin()`.
    reverse_iterator rend() {
        return reverse_iterator(static_cast<Derived&>(*this).begin());
    }

    /// Returns a `const_reverse_iterator` to the beginning of the container.
    ///
    /// Depends on `end() const`.
    const_reverse_iterator rbegin() const {
        return reverse_iterator(static_cast<const Derived&>(*this).end());
    }

    /// Returns a `const_reverse_iterator` to the end of the container.
    ///
    /// Depends on `begin() const`.
    const_reverse_iterator rend() const {
        return reverse_iterator(static_cast<const Derived&>(*this).begin());
    }

    /// Returns a `const_reverse_iterator` to the beginning of the container.
    ///
    /// Depends on `rbegin() const`.
    const_reverse_iterator crbegin() const {
        return static_cast<const Derived&>(*this).rbegin();
    }

    /// Returns a `const_reverse_iterator` to the end of the container.
    ///
    /// Depends on `rend() const`.
    const_reverse_iterator crend() const {
        return static_cast<const Derived&>(*this).rend();
    }

    /// Accesses the element at a given index with bounds-checking.
    ///
    /// Depends on `operator[] const`.
    const_reference at(size_type index) const {
        // First check is not strictly needed, but can be useful in case
        // `size_type` happens to be a signed type.
        const Derived& dthis = static_cast<const Derived&>(this);
        if (index < size_type() || index >= dthis.size())
            throw std::out_of_range("index out of range");
        return dthis[index];
    }

    /// Accesses the element at a given index with bounds-checking.
    ///
    /// Depends on `operator[]`.
    reference at(size_type index) {
        // First check is not strictly needed, but can be useful in case
        // `size_type` happens to be a signed type.
        Derived& dthis = static_cast<Derived&>(*this);
        if (index < size_type() || index >= dthis.size())
            throw std::out_of_range("index out of range");
        return dthis[index];
    }

    /// Accesses the element at a given index.
    ///
    /// Depends on `begin() const` and calls `std::advance` with ADL.
    const_reference operator[](size_type index) const {
        using std::advance;
        const_iterator it = static_cast<const Derived&>(*this).begin();
        advance(it, index);
        return *it;
    }

    /// Accesses the element at a given index.
    ///
    /// Depends on `begin()` and calls `std::advance` with ADL.
    reference operator[](size_type index) {
        using std::advance;
        iterator it = static_cast<Derived&>(*this).begin();
        advance(it, index);
        return *it;
    }

protected:

    /// Returns an `iterator` to the beginning of the container.
    ///
    /// Depends on `data()`.  This function is provided so that the non-const
    /// `begin()` can be overridden without shadowing the const `begin()`
    /// provided by this base type.
    iterator _begin() {
        return static_cast<Derived&>(*this).data();
    }

    /// Returns an `iterator` to the end of the container.
    ///
    /// Depends on `begin()` and `size()`.  This function is provided so that
    /// the non-const `end()` can be overridden without shadowing the const
    /// `end()` provided by this base type.
    iterator _end() {
        return static_cast<Derived&>(*this).begin()
             + static_cast<Derived&>(*this).size();
    }

    ~container_base() {}; // don't try to initialize a CRTP base type
};

/// An container-like type that two iterators as a range.
template<class InputIterator>
struct iterator_range
    : container_base<iterator_range<InputIterator>, InputIterator> {
    typedef InputIterator iterator_type;
    iterator_range(const iterator_type& first, const iterator_type& last)
        : first(first), last(last) {}
    iterator_type first;
    iterator_type last;
    iterator_type begin() const { return first; }
    iterator_type end()   const { return last;  }
};

/// Constructs an `iterator_range`.
template<class InputIterator> inline
iterator_range<InputIterator>
make_range(const InputIterator& first, const InputIterator& last) {
    return iterator_range<InputIterator>(first, last);
}

/// Constructs an `iterator_range` of integers from `T()` to `end`.
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
    typedef InputIterator iterator;

    /// Iterator category.
    typedef CALICO_HIDE(
        typename std::iterator_traits<iterator>::iterator_category
    ) iterator_category;

    /// Difference type.
    typedef CALICO_HIDE(
        typename std::iterator_traits<iterator>::difference_type
    ) difference_type;

    /// Value type.
    typedef CALICO_HIDE(
        typename std::result_of<
            UnaryOperation(
                typename std::iterator_traits<iterator>::reference
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
        const iterator& it,
        const UnaryOperation& op
    ) : _it(it), _op(op) {}

    /// Returns the underlying iterator.
    const iterator& base() const { return _it; }

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
        decltype(--std::declval<iterator&>()),
        transform_iterator&
    ) operator--() {
        --_it;
        return *this;
    }

    /// Post-decrements the iterator.
    CALICO_VALID_TYPE(
        decltype(std::declval<iterator&>()--),
        transform_iterator
    ) operator--(int) {
        transform_iterator t = *this;
        --*this;
        return t;
    }

    /// Advances the iterator by `n`.
    CALICO_VALID_TYPE(
        decltype(std::declval<iterator&>() +=
                 std::declval<difference_type>()),
        transform_iterator&
    ) operator+=(difference_type n) {
        _it += n;
        return *this;
    }

    /// Advances the iterator by `n` in reverse.
    CALICO_VALID_TYPE(
        decltype(std::declval<iterator&>() -=
                 std::declval<difference_type>()),
        transform_iterator&
    ) operator-=(difference_type n) {
        _it -= n;
        return *this;
    }

private:
    iterator _it;
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

/// Constructs a `transform_iterator`.
template<class InputIterator, class UnaryOperation> inline
transform_iterator<InputIterator, UnaryOperation>
make_transform_iterator(const InputIterator& it, const UnaryOperation& op) {
    return transform_iterator<InputIterator, UnaryOperation>(it, op);
}

/// Applies a given function to every element of an iterator range and returns
/// the result as a lazily evaluated iterable container (i.e. the "map"
/// function).
template<class InputIterator, class UnaryOperation> inline
auto transform(const InputIterator& first,
               const InputIterator& last,
               const UnaryOperation& op)
#ifndef CALICO_DOC_ONLY
-> decltype(make_range(make_transform_iterator(first, op),
                       make_transform_iterator(last, op)))
#endif
{   return  make_range(make_transform_iterator(first, op),
                       make_transform_iterator(last, op)); }

/// Applies a given function to every element of an iterable container and
/// returns the result as a lazily evaluated iterable container (i.e. the
/// "map" function).
template<class Container, class UnaryOperation> inline
auto transform(const Container& c, const UnaryOperation& op)
#ifndef CALICO_DOC_ONLY
-> decltype(transform(_priv::adl_begin(c), _priv::adl_end(c), op))
#endif
{   return  transform(_priv::adl_begin(c), _priv::adl_end(c), op); }

/// Reverses a container or iterator range.
///
/// Requires `rbegin` and `rend` as member functions.  This may be relaxed in
/// the future when C++14 support is improved.
template<class Container> inline
auto reverse_range(const Container& c)
#ifndef CALICO_DOC_ONLY
-> decltype(make_range(_priv::adl_rbegin(c), _priv::adl_rend(c)))
#endif
{   return  make_range(_priv::adl_rbegin(c), _priv::adl_rend(c)); }

namespace _priv {
// Initialize the tuple to the form `(0, 1, 2, ..., N2 + 1)`.
template<class T, std::size_t N2, std::size_t I = 0>
struct sord_tuple_iterator_helper_init {
    static void apply(n_tuple_t<T, N2 + 2>& tup) {
        std::get<I + 1>(tup) = std::get<I>(tup) + 1;
        sord_tuple_iterator_helper_init<T, N2, I + 1>::apply(tup);
    }
};
// Initialize the last element.
template<class T, std::size_t N2>
struct sord_tuple_iterator_helper_init<T, N2, N2> {
    static void apply(n_tuple_t<T, N2 + 2>& tup) {
        std::get<N2 + 1>(tup) = std::get<N2>(tup) + 1;
    }
};
// Special case where `N == 1`.
template<class T>
struct sord_tuple_iterator_helper_init<T, static_cast<std::size_t>(-1), 0> {
    static void apply(n_tuple_t<T, 1>&) {}
};
// Special case where `N == 0`.
template<class T>
struct sord_tuple_iterator_helper_init<T, static_cast<std::size_t>(-2), 0> {
    static void apply(n_tuple_t<T, 0>&) {}
};
// Retrieves the element just before `I`.
template<class T, std::size_t N, std::size_t I = 0>
struct sord_tuple_iterator_helper_getprev {
    static T apply(const n_tuple_t<T, N>& tup) {
        return std::get<I - 1>(tup);
    }
};
// Use the default-initialized value minus one if it doesn't exist.
template<class T, std::size_t N>
struct sord_tuple_iterator_helper_getprev<T, N, 0> {
    static T apply(const n_tuple_t<T, N>&) {
        return T() - 1;
    }
};
// Increments the tuple (of size `N1 + 1`).  If capped, reset this one and
// cascade to the next one (and so forth).
template<class T, std::size_t N1, std::size_t I = 0>
struct sord_tuple_iterator_helper_next {
    static void apply(n_tuple_t<T, N1 + 1>& tup, bool& end, const T& max) {
        ++std::get<I>(tup);
        if (std::get<I>(tup) == std::get<I + 1>(tup)) {
            std::get<I>(tup) = 1 +
                sord_tuple_iterator_helper_getprev<T, N1 + 1, I>::apply(tup);
            sord_tuple_iterator_helper_next<T, N1, I + 1>::apply(tup, end, max);
        }
    }
};
// Last one; if capped, we've reached the end.
template<class T, std::size_t N1>
struct sord_tuple_iterator_helper_next<T, N1, N1> {
    static void apply(n_tuple_t<T, N1 + 1>& tup, bool& end, const T& max) {
        ++std::get<N1>(tup);
        if (std::get<N1>(tup) == max)
            end = true;
    }
};
// Special case where `N == 0`.
template<class T>
struct sord_tuple_iterator_helper_next<T, static_cast<std::size_t>(-1), 0> {
    static void apply(n_tuple_t<T, 0>&, bool&, const T&) {}
};
}
// Tuple iterator used by `iterate_sord`.
template<class T, std::size_t N>
struct sord_tuple_iterator
  : input_iterator_base<
        const sord_tuple_iterator<T, N>,
        n_tuple_t<T, N>
    > {
    // Initialize the iterator.  If `N == 0` then there are no elements so the
    // iterator is already at the end.
    sord_tuple_iterator(T max, bool end = false) : max(max), end(end || !N) {
        _priv::sord_tuple_iterator_helper_init<T, N - 2>::apply(tup);
    }
    sord_tuple_iterator& operator++() {
        _priv::sord_tuple_iterator_helper_next<T, N - 1>::apply(tup, end, max);
        return *this;
    }
    const n_tuple_t<T, N>& operator*() const { return tup; }
    friend bool operator==(const sord_tuple_iterator& i,
                           const sord_tuple_iterator& j) {
        return i.end && j.end;
    }
private:
    T max;
    bool end;
    n_tuple_t<T, N> tup;
};

/// Iterate over all natural number `N`-tuples with a strict ordering imposed
/// on the elements.
///
/// For a tuple of the form `(x[0], ... x[N - 1])`, the requirement is `x[i] <
/// x[i + 1]` for all `i < N - 1`.  The iterator returns tuples in this order
/// such that the first element is the most rapidly varying element.  For
/// example:
///
///     (0, 1, 2, 3)
///     (0, 1, 2, 4)
///     (0, 1, 3, 4)
///     (0, 2, 3, 4)
///     (1, 2, 3, 4)
///     (0, 1, 2, 5)
///     (0, 1, 3, 5)
///     (0, 2, 3, 5)
///     (1, 2, 3, 5)
///     (0, 1, 4, 5)
///     (0, 2, 4, 5)
///     (1, 2, 4, 5)
///     (0, 3, 4, 5)
///     (1, 3, 4, 5)
///     (2, 3, 4, 5)
///
template<std::size_t N, class T>
inline iterator_range<sord_tuple_iterator<T, N> > iterate_sord(T max) {
    return make_range(
        sord_tuple_iterator<T, N>(max),
        sord_tuple_iterator<T, N>(max, true)
    );
}

}
#endif
