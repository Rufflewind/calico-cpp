#ifndef POWYVPDQOXWEKKTYZEBL
#define POWYVPDQOXWEKKTYZEBL
/// @file
///
/// `Lens`es are proxy objects that allow a value to be accessed and mutated
/// via a pair of functions ("get" and "put").
///
/// The concept is named after the Haskell lens package.  `Lens`es are not
/// required to be `Copyable`, only `Movable`.  They must allow the following
/// operations:
///
/// ~~~~cpp
///
///     // This is the "get" function ("getter").
///     operator Lens::value_type() const;
///
///     // This is the "put" function ("setter").
///     Lens& operator=(const Lens::value_type& x);
///
///     // This is the rvalue-version of the "put" function (optional).
///     Lens& operator=(T&& x);
///
/// ~~~~
///
#include <type_traits>
#include <utility>
namespace cal {
namespace _priv {
// `T` is assumed to be a nonreference type.
// (1) Default to `T` if `T::value_type` is missing.
template<class T, class = void> struct lens_traits {
    typedef T value_type;
};
// (2) Use `T::value_type` if `T::value_type` is present
template<class T> struct lens_traits
<T, typename std::conditional<0, typename T::value_type, void>::type> {
    typedef typename T::value_type value_type;
};
}

/// Traits class for the `Lens` concept.
template<class Lens>
struct lens_traits
#ifndef CALICO_DOC_ONLY
    : _priv::lens_traits<typename std::remove_reference<Lens>::type> {};
#else
{
    /// The apparent value type of the `Lens`.
    ///
    /// This is the type returned by the "get" function and received by the
    /// "put" function.
    typedef auto value_type;
};
#endif

/// CRTP base type for constructing `Lens` types.
///
/// @tparam Lens  The `Lens` that is being derived.
/// @tparam T     The value type of the `Lens`.
///
template<class Lens, class T>
struct lens_base {

    /// The value type of the lens.
    typedef T value_type;

    /// Pre-increments through the lens.
    Lens& operator++() {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis += 1;
    }

    /// Pre-decrements through the lens.
    Lens& operator--() {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis -= 1;
    }

    /// Addition assignment through the lens.
    Lens& operator+=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) + x;
    }

    /// Subtraction assignment through the lens.
    Lens& operator-=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) - x;
    }

    /// Multiplication assignment through the lens.
    Lens& operator*=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) * x;
    }

    /// Division assignment through the lens.
    Lens& operator/=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) / x;
    }

    /// Modulo assignment through the lens.
    Lens& operator%=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) % x;
    }

    /// Bitwise AND assignment through the lens.
    Lens& operator&=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) & x;
    }

    /// Bitwise OR assignment through the lens.
    Lens& operator|=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) | x;
    }

    /// Bitwise XOR assignment through the lens.
    Lens& operator^=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) ^ x;
    }

    /// Bitwise left shift assignment through the lens.
    Lens& operator<<=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) << x;
    }

    /// Bitwise right shift assignment through the lens.
    Lens& operator>>=(const value_type& x) {
        Lens* dthis = static_cast<Lens*>(this);
        return *dthis = static_cast<value_type>(*dthis) >> x;
    }

};

/// A lens that negates the value before storing or accessing it.
///
/// If `L` is the `negating_lens` and `x` is the internal lens, the following
/// relationships are satisfied:
///
///     L        ==   -x
///     L  = c  <==>   x  = -c
///     L += c  <==>   x -=  c
///     L -= c  <==>   x +=  c
///
/// The addition/subtraction-assignment operators are overloaded here to
/// ensure better optimization for floating-point numbers (when `-ffast-math`
/// or its equivalent is not enabled).
template<class Lens, class T = typename lens_traits<Lens>::value_type>
struct negating_lens : lens_base<negating_lens<Lens, T>, T> {

    /// The value type accepted by the lens.
    typedef T value_type;

    /// The type of the lens being wrapped.
    typedef Lens internal_type;

    /// Wraps a lens.
    negating_lens(internal_type&& data)
        : _data(std::forward<internal_type>(data)) {}

    /// Returns the internal lens.
    internal_type data() { return _data; };

    /// Returns the internal lens.
    const internal_type& data() const { return _data; };

    /// Gets the value through the lens.
    operator value_type() const { return -static_cast<value_type>(data()); }

    /// Puts a value through the lens.
    negating_lens& operator=(const value_type& x) {
        data() = -x;
        return *this;
    }

    /// Puts a value through the lens (rvalue version).
    negating_lens& operator=(value_type&& x) {
        data() = -std::move(x);
        return *this;
    }

    /// Addition assignment through the lens.
    negating_lens& operator+=(const value_type& x) {
        data() -= x;
        return *this;
    }

    /// Subtraction assignment through the lens.
    negating_lens& operator-=(const value_type& x) {
        data() += x;
        return *this;
    }

private:
    internal_type _data;
};

/// Constructs a `negating_lens` from a given value.
template<class T> inline negating_lens<T> make_negating_lens(T&& x) {
    return negating_lens<T>(std::forward<T>(x));
}

/// A lens that scales the value by a factor before storing or accessing it.
///
/// If `L` is the `scaling_lens`, `x` is the internal lens, and `k` is the
/// factor to be multiplied, the following relationships are satisfied:
///
///     L       ==    x / k
///     L = c  <==>   x = c * k
///
template<class Lens, class Factor,
         class T = typename lens_traits<Lens>::value_type>
struct scaling_lens : lens_base<scaling_lens<Lens, Factor, T>, T> {

    /// The value type accepted by the lens.
    typedef T value_type;

    /// The type of the factor to be multiplied.
    typedef Factor factor_type;

    /// The type of the lens being wrapped.
    typedef Lens internal_type;

    /// Wraps a lens.
    scaling_lens(internal_type&& data, factor_type&& factor)
        : _data(std::forward<internal_type>(data)),
          _factor(std::forward<factor_type>(factor)) {}

    /// Returns the internal lens.
    internal_type data() { return _data; };

    /// Returns the internal lens.
    const internal_type& data() const { return _data; };

    /// Returns the factor to be multiplied.
    const factor_type& factor() const { return _factor; };

    /// Gets the value through the lens.
    operator value_type() const
    { return static_cast<value_type>(data()) / _factor; }

    /// Puts a value through the lens.
    scaling_lens& operator=(const value_type& x) {
        data() = x * _factor;
        return *this;
    }

    /// Puts a value through the lens (rvalue version).
    scaling_lens& operator=(value_type&& x) {
        data() = std::move(x) * _factor;
        return *this;
    }

private:
    factor_type _factor;
    internal_type _data;
};

/// Constructs a `scaling_lens` from a given value.
template<class T, class U> inline
scaling_lens<T, U> make_scaling_lens(T&& lens, U&& factor) {
    return scaling_lens<T, U>(std::forward<T>(lens), std::forward<U>(factor));
}

}
#endif
