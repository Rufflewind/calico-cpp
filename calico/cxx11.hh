#ifndef FZ_CXX11_H
#define FZ_CXX11_H
#include <climits>
#include <cstddef>
#include <iterator>
// Header that provides basic features from C++11 and feature-testing macros.
//
// The following macros attempt to determine the availability of C++11
// features.  However, __cplusplus is unreliable on GCC versions prior to 4.7.
// Instead, GCC defines the __GXX_EXPERIMENTAL_CXX0X__ macro when -std=c++0x
// is used.  C++11 support in those versions are also somewhat incomplete.
//
// For compilers other than GCC / MSVC, the HAVE_... macros will need to be
// manually specified.
//
// For more info: http://gcc.gnu.org/projects/cxx0x.html
#if __cplusplus >= 201103L && !defined(HAVE_CXX11)
#   define HAVE_CXX11                   // This macro turns on *all* features
#endif
#if (defined(HAVE_CXX11) || _MSC_VER >= 1500       \
     || (defined(__GXX_EXPERIMENTAL_CXX0X__)       \
         && __GNUC__ >= 4 && __GNUC_MINOR__ >= 2)) \
    && !defined(HAVE_TYPE_TRAITS)
#   define HAVE_TYPE_TRAITS
#endif
#if (defined(HAVE_CXX11) || _MSC_VER >= 1700       \
     || (defined(__GXX_EXPERIMENTAL_CXX0X__)       \
         && __GNUC__ >= 4 && __GNUC_MINOR__ >= 6)) \
    && !defined(HAVE_FOR_RANGE)                    \
    && !defined(__clang__)
// Note that __has_feature(cxx_range_for) won't work here because it doesn't
// detect the existence of std::begin/end.
#   define HAVE_FOR_RANGE
#endif
#if (defined(HAVE_CXX11) || _MSC_VER >= 1700       \
     || defined(__GXX_EXPERIMENTAL_CXX0X__))       \
    && !defined(HAVE_CSTDINT)
#   define HAVE_CSTDINT
#endif
#if (defined(HAVE_CXX11) || _MSC_VER >= 1600       \
    || (defined(__GXX_EXPERIMENTAL_CXX0X__)        \
        && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3))  \
    && !defined(HAVE_STATIC_ASSERT)
#   define HAVE_STATIC_ASSERT
#endif
#if (defined(HAVE_CXX11) || _MSC_VER >= 1600       \
     || (defined(__GXX_EXPERIMENTAL_CXX0X__)       \
         && __GNUC__ >= 4 && __GNUC_MINOR__ >= 4)) \
    && !defined(HAVE_TRAILING_RETURN)
#   define HAVE_TRAILING_RETURN
#endif
#if (defined(HAVE_CXX11)                           \
     || (defined(__GXX_EXPERIMENTAL_CXX0X__)       \
         && __GNUC__ >= 4 && __GNUC_MINOR__ >= 6)) \
    && !defined(HAVE_CONSTEXPR)
#   define HAVE_CONSTEXPR
#else
#   define constexpr
#endif
#if (defined(HAVE_CXX11)                           \
     || (defined(__GXX_EXPERIMENTAL_CXX0X__)       \
         && __GNUC__ >= 4 && __GNUC_MINOR__ >= 6)) \
    && !defined(HAVE_NOEXCEPT)
#   define HAVE_NOEXCEPT
#else
#   define noexcept
#   define noexcept(x)
#endif

// Type traits is only available on C++11
#ifdef HAVE_TYPE_TRAITS
#   include <type_traits>
#endif

// Determine some commonly used integer types.  Prefer <cstdint> or <stdint.h>
// if available; otherwise, figure it out the hard way.
#ifdef __GNUC__
#   define HAVE_STDINT_H
#endif
#ifdef HAVE_CSTDINT
#   include <cstdint>
#   define HAVE_UINT_FAST16 std::uint_fast16_t
#   define HAVE_UINT32 std::uint32_t
#   define HAVE_UINT64 std::uint64_t
#elif defined(HAVE_STDINT_H)
#   include <stdint.h>
#   define HAVE_UINT_FAST16 ::uint_fast16_t
#   ifdef INT32_MAX
#       define HAVE_UINT32 ::uint32_t
#   endif
#   ifdef INT64_MAX
#       define HAVE_UINT64 ::uint64_t
#   endif
#else
#   define HAVE_UINT_FAST16 unsigned
#   if ULLONG_MAX == 0xffffffff
#       define HAVE_UINT32 unsigned long long
#   elif ULONG_MAX == 0xffffffff
#       define HAVE_UINT32 unsigned long
#   elif UINT_MAX == 0xffffffff
#       define HAVE_UINT32 unsigned
#   elif USHORT_MAX == 0xffffffff
#       define HAVE_UINT32 unsigned short
#   endif
#   if ULLONG_MAX == 0xffffffffffffffff
#       define HAVE_UINT64 unsigned long long
#   elif ULONG_MAX == 0xffffffffffffffff
#       define HAVE_UINT64 unsigned long
#   elif UINT_MAX == 0xffffffffffffffff
#       define HAVE_UINT64 unsigned
#   elif USHORT_MAX == 0xffffffffffffffff
#       define HAVE_UINT64 unsigned short
#   endif
#   if !defined(HAVE_UINT32) && !defined(HAVE_UINT64)
#       error "Can't find a 32-bit nor 64-bit unsigned integer type."
#   endif
#endif

// Static assertion macro; in some cases it may be necessary to protect the
// expression with parentheses if the preprocessor interprets it incorrectly
#if !defined(HAVE_STATIC_ASSERT) && !defined(static_assert)
#   define FZ_STATIC_ASSERT_MSG(line) FZ_STATIC_ASSERT_MSG2(line)
#   define FZ_STATIC_ASSERT_MSG2(line) static_assertion_failed_at_line_ ## line
#   define static_assert(expression, message)                           \
        enum { FZ_STATIC_ASSERT_MSG(__LINE__) = 1 / (int) (expression) }
#endif

/// Namespace `fz`.
namespace fz {
namespace _priv { template<class T, class = void> struct get_iterator; }

/// @addtogroup FzCxx11Utility Utility functions
///
/// Provided as an extension to the C++11 shims (not part of the C++11
/// Standard Library).
///
/// @{

/// Constructs a type that corresponds to `T` but with the same const-volatile
/// qualifiers as the given `Target` type.
template<class T, class Target>
struct match_cv { typedef T type; };
template<class T, class Target>
struct match_cv<T, const Target> { typedef const T type; };
template<class T, class Target>
struct match_cv<T, volatile Target> { typedef volatile T type; };
template<class T, class Target>
struct match_cv<T, const volatile Target> { typedef const volatile T type; };

/// Deduces the iterator type of the given container.
template<class T>
struct get_iterator { typedef typename _priv::get_iterator<T>::type type; };

/// @}

#ifdef HAVE_TYPE_TRAITS
using std::integral_constant;
using std::false_type;
using std::true_type;

using std::is_const;
using std::is_volatile;
using std::add_const;
using std::add_volatile;
using std::add_cv;
using std::remove_const;
using std::remove_volatile;
using std::remove_cv;
using std::make_signed;

using std::conditional;
using std::enable_if;
using std::is_same;
using std::is_base_of;
#else

/// Represents a compile-time constant of integral type (equivalent to
/// `std::integral_constant`).
template<class T, T Value>
struct integral_constant {
    /// Type of the `value`.
    typedef T value_type;
    /// Type of the `integral_constant`.
    typedef integral_constant type;
    /// Value stored in this type.
    static const value_type value = Value;
    /// Converts the integral constant type into its constant value.
    operator value_type() const { return value; }
};
/// An alias to `integral_constant` with a `value` of `true` (equivalent to
/// `std::true_type`).
typedef integral_constant<bool, true> true_type;
/// An alias to `integral_constant` with a `value` of `false` (equivalent to
/// `std::false_type`).
typedef integral_constant<bool, false> false_type;

// Various cv-qualifier templates
template<class T> struct is_const : false_type {};
template<class T> struct is_const<const T> : true_type {};
template<class T> struct is_volatile : false_type {};
template<class T> struct is_volatile<volatile T> : true_type {};
template<class T> struct add_const { typedef const T type; };
template<class T> struct add_volatile { typedef volatile T type; };
template<class T> struct add_cv { typedef const volatile T type; };
template<class T> struct remove_const { typedef T type; };
template<class T> struct remove_const<const T> { typedef T type; };
template<class T> struct remove_volatile { typedef T type; };
template<class T> struct remove_volatile<volatile T> { typedef T type; };
template<class T> struct remove_cv { typedef T type; };
template<class T> struct remove_cv<const T> { typedef T type; };
template<class T> struct remove_cv<volatile T> { typedef T type; };
template<class T> struct remove_cv<const volatile T> { typedef T type; };

// Makes an integral type a signed type [partial implementation].
namespace _priv {
template<class T> struct make_signed {};
template<> struct make_signed<unsigned char> { typedef signed char type; };
template<> struct make_signed<unsigned short> { typedef short type; };
template<> struct make_signed<unsigned> { typedef int type; };
template<> struct make_signed<unsigned long> { typedef long type; };
template<> struct make_signed<unsigned long long> { typedef long long type; };
}
template<class T> struct make_signed {
    typedef typename match_cv<
        typename _priv::make_signed<
            typename remove_cv<T>::type>::type, T>::type type;
};

// Template that selectively enables overloads.
template<bool Condition, class T = void> struct enable_if {};
template<class T> struct enable_if<true, T> { typedef T type; };

// Returns a type based on a boolean condition.
template<bool Condition, class TrueT, class FalseT>
struct conditional { typedef FalseT type; };
template<class TrueT, class FalseT>
struct conditional<true, TrueT, FalseT> { typedef TrueT type; };

// Checks if types are the same.
template<class T1, class T2> struct is_same : false_type {};
template<class T> struct is_same<T, T> : true_type {};

// Checks if a type is derived from another type.
namespace _priv {
typedef char y[1];
typedef char n[2];
template<class Base, class T> struct is_base_of {
    struct conv { operator Base*() const; operator T*(); };
    template<class Int>
    static y& check(T*, Int);
    static n& check(Base*, int);
    static const bool value = sizeof(check(conv(), 0)) == sizeof(y);
};
template<class T> struct is_base_of<T, T> { static const bool value = true; };
}
template<class Base, class Derived>
struct is_base_of
    : integral_constant<bool, _priv::is_base_of<Base, Derived>::value> {};

#endif

// Integer types
typedef HAVE_UINT_FAST16 uint_fast16_t;
#ifdef HAVE_UINT32
typedef HAVE_UINT32 uint32_t;
#endif
#ifdef HAVE_UINT64
typedef HAVE_UINT64 uint64_t;
#endif

#ifdef HAVE_FOR_RANGE
using std::begin;
using std::end;
namespace _priv {
template<class T, class>
struct get_iterator { typedef decltype(begin(std::declval<T>())) type; };
}
#else
namespace _priv {

template<class T, class>
struct get_iterator { typedef typename T::iterator type; };
template<class T, class X>
struct get_iterator<T&, X> { typedef typename T::iterator type; };
template<class T, class X>
struct get_iterator<const T&, X> { typedef typename T::const_iterator type; };
template<class T, std::size_t N, class X>
struct get_iterator<T (&)[N], X> { typedef T* type; };
template<class T, std::size_t N, class X>
struct get_iterator<const T (&)[N], X> { typedef const T* type; };

// Let `std::pair<Iterator, Iterator>` be treated as an iterable container.
template<class Iterator>
struct get_iterator<std::pair<Iterator, Iterator>,
    typename conditional<true, void,
        typename std::iterator_traits<Iterator>::iterator_category>::type> {
    typedef Iterator type;
};
template<class Iterator>
struct get_iterator<std::pair<Iterator, Iterator>&,
    typename conditional<true, void,
        typename std::iterator_traits<Iterator>::iterator_category>::type> {
    typedef Iterator type;
};
template<class Iterator>
struct get_iterator<const std::pair<Iterator, Iterator>&,
    typename conditional<true, void,
        typename std::iterator_traits<Iterator>::iterator_category>::type> {
    typedef Iterator type;
};

}

template<class C>
typename get_iterator<const C&>::type begin(const C& c) { return c.begin(); }
template<class C>
typename get_iterator<const C&>::type end(const C& c) { return c.end(); }
template<class C>
typename get_iterator<C&>::type begin(C& c) { return c.begin(); }
template<class C>
typename get_iterator<C&>::type end(C& c) { return c.end(); }
template<class T, std::size_t N>
typename get_iterator<T (&)[N]>::type begin(T (&array)[N]) { return array; }
template<class T, std::size_t N>
typename get_iterator<T (&)[N]>::type end(T (&array)[N]) { return array + N; }
#endif

}

namespace std {
// Let `std::pair<Iterator, Iterator>` be treated as an iterable container.
template<class Iterator>
typename fz::conditional<true, Iterator,
    typename std::iterator_traits<Iterator>::iterator_category>::type
begin(const std::pair<Iterator, Iterator>& p) { return p.first; }
template<class Iterator>
typename fz::conditional<true, Iterator,
    typename std::iterator_traits<Iterator>::iterator_category>::type
end(const std::pair<Iterator, Iterator>& p) { return p.second; }
}
#endif
