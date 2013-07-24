#ifndef FZ_CXX11_HH
#define FZ_CXX11_HH
#include <climits>
#include <cstddef>
#include <iterator>
#include <utility>

// This header provides some basic features from C++11 and a few rudimentary
// feature-testing macros.  It is incomplete and does not support every
// compiler.  Full feature testing is best provided by actual build tools.
//
// Some useful information can be found here:
//
//     http://wiki.apache.org/stdcxx/C++0xCompilerSupport
//     http://clang.llvm.org/docs/LanguageExtensions.html
//     http://gcc.gnu.org/projects/cxx0x.html
//
// For MSVC, the version macro evaluates to:
//
//     _MSC_VER = 1700 for Visual Studio 2012 / 11.0
//     _MSC_VER = 1600 for Visual Studio 2010 / 10.0
//     _MSC_VER = 1500 for Visual Studio 2008 /  9.0
//
// The following set of macros determine the availability of C++11 features in
// some common compilers (assuming they even have these kinds of things
// properly documented, which is often not the case).  Note that despite what
// the standard says, the __cplusplus macro should not be fully trusted as
// compilers can and will lie about it (e.g. Clang).
//
// If the compiler does in fact support *every* feature of C++11 correctly
// (quite unlikely) then feel free to define the HAVE_CXX11 macro; otherwise,
// define the individual HAVE_... feature macros instead.

// Global toggle
#if __cplusplus >= 201103L \
    && !defined(__clang__) // Clang does not fully support C++11
#   undef  HAVE_CXX11
#   define HAVE_CXX11 1
#endif

// Macros to reduce typing.  They should be #undef'ed afterwards.
#ifdef DOC_ONLY
#   define __has_feature(x) 0
#endif
#define CLANG_FEATURE(feature) (defined(__clang__) && __has_feature(feature))
#define GCC_VERSION(major, minor)                                       \
    (defined(__GXX_EXPERIMENTAL_CXX0X__)                                \
     && ( __GNUC__ == major && __GNUC_MINOR__ >= minor || __GNUC__ > major))

// Language features
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_auto_type)             \
    || GCC_VERSION(4, 4)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_AUTO
#   define HAVE_AUTO 1
#endif
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_constexpr)             \
    || GCC_VERSION(4, 6)
#   undef  HAVE_CONSTEXPR
#   define HAVE_CONSTEXPR 1
#else
#   undef  constexpr
#   define constexpr
#endif
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_decltype)              \
    || GCC_VERSION(4, 3)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_DECLTYPE
#   define HAVE_DECLTYPE 1
#else
#   undef  constexpr
#   define constexpr
#endif
#if defined(HAVE_CXX11)                                         \
    || GCC_VERSION(4, 3)                                        \
    || CLANG_FEATURE(cxx_default_function_template_args)
#   undef  HAVE_DEFAULT_FUNC_TEMPLATE_ARG
#   define HAVE_DEFAULT_FUNC_TEMPLATE_ARG 1
#endif
#if defined(HAVE_CXX11)                         \
    || GCC_VERSION(4, 6)                        \
    || CLANG_FEATURE(cxx_noexcept)
#   undef  HAVE_NOEXCEPT
#   define HAVE_NOEXCEPT 1
#else
#   undef  noexcept
#   define noexcept
#endif
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_range_for)             \
    || GCC_VERSION(4, 6)                        \
    || _MSC_VER >= 1700
#   undef  HAVE_RANGE_FOR
#   define HAVE_RANGE_FOR 1
#endif
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_rvalue_references)     \
    || GCC_VERSION(4, 3)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_RVALUE
#   define HAVE_RVALUE 1
#endif
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_static_assert)         \
    || GCC_VERSION(4, 3)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_STATIC_ASSERT
#   define HAVE_STATIC_ASSERT 1
#endif
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_alias_templates)       \
    || GCC_VERSION(4, 7)
#   undef  HAVE_TEMPLATE_ALIAS
#   define HAVE_TEMPLATE_ALIAS 1
#endif
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_trailing_return)       \
    || GCC_VERSION(4, 4)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_TRAILING_RETURN
#   define HAVE_TRAILING_RETURN 1
#endif
#if defined(HAVE_CXX11)                         \
    || CLANG_FEATURE(cxx_variadic_templates)    \
    || GCC_VERSION(4, 3)                        \
    || _MSC_VER >= 1700
#   undef  HAVE_VARIADIC_TEMPLATE
#   define HAVE_VARIADIC_TEMPLATE 1
#endif
#ifndef HAVE_LONG_LONG               // Practically all compilers support this
#   define HAVE_LONG_LONG 1
#endif

// Library features
#if defined(HAVE_CXX11)
#   undef  HAVE_ADDRESSOF
#   define HAVE_ADDRESSOF 1
#endif
#if defined(HAVE_CXX11)                         \
    || defined(__GXX_EXPERIMENTAL_CXX0X__)      \
    || _MSC_VER >= 1700
#   undef  HAVE_CSTDINT
#   define HAVE_CSTDINT 1
#endif
#if defined(HAVE_CXX11)                         \
    || GCC_VERSION(4, 2)                        \
    || _MSC_VER >= 1500
#   undef  HAVE_TYPE_TRAITS
#   define HAVE_TYPE_TRAITS 1
#endif
#undef CLANG_FEATURE
#undef GCC_VERSION

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

// For documentation purposes
#ifdef DOC_ONLY
#define HAVE_RVALUE
#define HAVE_UINT32
#define HAVE_UINT64
#endif

/// Namespace `fz`.
namespace fz {
namespace _priv { template<class T, class = void> struct get_iterator; }

/// @addtogroup FzUtility Utilities
///
/// Miscellaneous utility functions.
///
/// These are provided as an extension to the C++11 compatibility features and
/// are not part of the C++11 standard library.
///
/// @{

/// Obtains a type related to `T` with the same const-volatile qualifiers as
/// `Target`.
template<class T, class Target>
struct match_cv { typedef T type; };
template<class T, class Target>
struct match_cv<T, const Target> { typedef const T type; };
template<class T, class Target>
struct match_cv<T, volatile Target> { typedef volatile T type; };
template<class T, class Target>
struct match_cv<T, const volatile Target> { typedef const volatile T type; };

/// Deduces the iterator type of a container.
template<class T>
struct get_iterator { typedef typename _priv::get_iterator<T>::type type; };

/// @}

/// @addtogroup FzCxx11 C++11
///
/// Implementation of various features from C++11 for compatibility purposes.
///
/// @{

/// Fastest 16-bit unsigned integer type.
typedef HAVE_UINT_FAST16 uint_fast16_t;

#ifdef HAVE_UINT32
/// Fastest 32-bit unsigned integer typed.
typedef HAVE_UINT32 uint32_t;
#endif

#ifdef HAVE_UINT64
/// Fastest 64-bit unsigned integer typed.
typedef HAVE_UINT64 uint64_t;
#endif

#ifdef HAVE_TYPE_TRAITS
using std::integral_constant;
using std::false_type;
using std::true_type;

using std::is_const;
using std::is_volatile;
using std::is_lvalue_reference;
using std::is_rvalue_reference;
using std::is_reference;
using std::add_const;
using std::add_volatile;
using std::add_cv;
using std::add_lvalue_reference;
using std::add_rvalue_reference;
using std::remove_const;
using std::remove_volatile;
using std::remove_cv;
using std::remove_reference;
using std::make_signed;

using std::conditional;
using std::enable_if;
using std::is_same;
using std::is_base_of;
#else

/// Represents a compile-time constant of integral type.
template<class T, T Value>
struct integral_constant {

    /// Type of the `value`.
    typedef T value_type;

    /// Type of the `integral_constant`.
    typedef integral_constant type;

    /// Value stored in this type.
    static const value_type value = Value;

    /// Converts the integral constant type into its constant value.
    constexpr operator value_type() const { return value; }
};

/// An alias to `integral_constant` with a `value` of `true`.
typedef integral_constant<bool, true> true_type;

/// An alias to `integral_constant` with a `value` of `false`.
typedef integral_constant<bool, false> false_type;

/// Returns whether the type is `const`-qualified.
template<class T> struct is_const : false_type {};
template<class T> struct is_const<const T> : true_type {};

/// Returns whether the type is `volatile`-qualified.
template<class T> struct is_volatile : false_type {};
template<class T> struct is_volatile<volatile T> : true_type {};

/// Returns whether the type is an lvalue or rvalue reference.
template<class T> struct is_lvalue_reference : false_type {};
template<class T> struct is_lvalue_reference<T&> : true_type {};

/// Returns whether the type is an rvalue reference.
template<class T> struct is_rvalue_reference : false_type {};
#ifdef HAVE_RVALUE
template<class T> struct is_rvalue_reference<T&&> : true_type {};
#endif

/// Returns whether the type is an lvalue or rvalue reference.
template<class T> struct is_reference : false_type {};
template<class T> struct is_reference<T&> : true_type {};
#ifdef HAVE_RVALUE
template<class T> struct is_reference<T&&> : true_type {};
#endif

/// Adds the `const` qualifier to the type.
template<class T> struct add_const { typedef const T type; };

/// Adds the `volatile` qualifier to the type.
template<class T> struct add_volatile { typedef volatile T type; };

/// Adds both the `const` and `volatile` qualifiers to the type.
template<class T> struct add_cv { typedef const volatile T type; };

namespace _priv {
template<class T, bool = std::is_void<T>::value>
struct add_lvalue_reference { typedef T& type; };
template<class T>
struct add_lvalue_reference<T, true> { typedef T type; };
}

/// Converts the type into an lvalue reference.
template<class T>
struct add_lvalue_reference {
    typedef typename _priv::add_lvalue_reference<T>::type type;
};

#ifdef HAVE_RVALUE
namespace _priv {
template<class T, bool = std::is_void<T>::value>
struct add_rvalue_reference { typedef T&& type; };
template<class T>
struct add_rvalue_reference<T, true> { typedef T type; };
}
#endif

/// Converts the type into an rvalue reference provided that `T` is not an
/// lvalue reference.
template<class T>
struct add_rvalue_reference {
    typedef typename _priv::add_rvalue_reference<T>::type type;
};

/// Removes the `const` qualifier from the type.
template<class T> struct remove_const { typedef T type; };
template<class T> struct remove_const<const T> { typedef T type; };

/// Removes the `volatile` qualifier from the type.
template<class T> struct remove_volatile { typedef T type; };
template<class T> struct remove_volatile<volatile T> { typedef T type; };

/// Removes both the `const and `volatile` qualifiers from the type.
template<class T> struct remove_cv { typedef T type; };
template<class T> struct remove_cv<const T> { typedef T type; };
template<class T> struct remove_cv<volatile T> { typedef T type; };
template<class T> struct remove_cv<const volatile T> { typedef T type; };

/// Converts to a non-reference type.
template<class T> struct remove_reference { typedef T type; };
template<class T> struct remove_reference<T&> { typedef T type; };
#ifdef HAVE_RVALUE
template<class T> struct remove_reference<T&&> { typedef T type; };
#endif

// Makes an integral type a signed type [partial implementation].
namespace _priv {
template<class T> struct make_signed {};
template<> struct make_signed<unsigned char> { typedef signed char type; };
template<> struct make_signed<unsigned short> { typedef short type; };
template<> struct make_signed<unsigned> { typedef int type; };
template<> struct make_signed<unsigned long> { typedef long type; };
#ifdef HAVE_LONG_LONG
template<> struct make_signed<unsigned long long> { typedef long long type; };
#endif
}

/// Converts an integral type to a signed type.
template<class T> struct make_signed {
    typedef typename match_cv<
        typename _priv::make_signed<
            typename remove_cv<T>::type>::type, T>::type type;
};

/// Contains a `type` member equal to `T` if `Condition` is `true`.
template<bool Condition, class T = void> struct enable_if {};
template<class T> struct enable_if<true, T> { typedef T type; };

/// Returns one of the types based on a boolean condition.
template<bool Condition, class TrueT, class FalseT>
struct conditional { typedef FalseT type; };
template<class TrueT, class FalseT>
struct conditional<true, TrueT, FalseT> { typedef TrueT type; };

/// Returns whether the types are the same.
template<class T, class U> struct is_same : false_type {};
template<class T> struct is_same<T, T> : true_type {};

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

/// Checks if a type is derived from another type.
template<class Base, class Derived>
struct is_base_of
    : integral_constant<bool, _priv::is_base_of<Base, Derived>::value> {};

#endif

#ifdef HAVE_ADDRESSOF
using std::addressof;
#else
/// Obtains the address of an object even if `operator&` is overloaded.
template<class T>
T* addressof(T& x) {
    return reinterpret_cast<T*>(
        &const_cast<char&>(reinterpret_cast<const volatile char&>(x)));
}
#endif

#ifdef HAVE_BEGIN_END
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

#ifdef HAVE_RVALUE
#ifdef HAVE_RVALUE_HELPERS
using std::declval;
using std::forward;
using std::move;
#else

/// Converts a type into an unevaluated operand.
template<class T>
typename add_rvalue_reference<T>::type declval() noexcept;

/// Perfectly forwards the given argument.
template<class T>
inline constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
    static_assert(!is_lvalue_reference<T>::value,
                  "template argument T cannot be an lvalue reference");
    return static_cast<T&&>(t);
}
template<class T>
inline constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

/// Converts an rvalue reference into an xvalue.
template<class T>
constexpr typename remove_reference<T>::type&& move(T&& t) noexcept {
    static_cast<typename remove_reference<T>::type&&>(t);
}

#endif
#endif

/// @}

namespace _priv {
template<class Iterator,
         class = typename std::iterator_traits<Iterator>::iterator_category>
struct ensure_iterator {
    typedef Iterator type;
};
}

}

namespace std {

/// @addtogroup FzUtility
///
/// @{

/// Returns the first item in the iterator pair.
template<class Iterator>
typename fz::_priv::ensure_iterator<Iterator>::type
begin(const std::pair<Iterator, Iterator>& p) { return p.first; }

/// Returns the second item in the iterator pair.
template<class Iterator>
typename fz::_priv::ensure_iterator<Iterator>::type
end(const std::pair<Iterator, Iterator>& p) { return p.second; }

/// @}

}
#endif
