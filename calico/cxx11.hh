#ifndef FZ_CXX11_HH
#define FZ_CXX11_HH
#include <climits>
#include <cstddef>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
/// @file
///
/// This header provides some basic features from C++11 and a few rudimentary
/// feature-testing macros.
///
/// The feature tests (or more accurately, *version* tests) in this header are
/// incomplete and do not support every compiler.  Full feature testing is
/// best provided by actual build tools.

/// @defgroup FzCxx11 C++11
///
/// Implementation of various features from C++11 for compatibility purposes.
///
/// A set of macros is used determine the availability of C++11 features in
/// some common compilers, assuming they even have these kinds of things
/// properly documented (which is often not the case).  Note that despite what
/// the standard says, the `__cplusplus` macro is not be trusted as compilers
/// can and will lie about it (e.g. Clang).
///
/// If the compiler does in fact support *every* feature of C++11 correctly
/// (quite unlikely) then feel free to define the `HAVE_CXX11` macro;
/// otherwise, use the individual `HAVE_`... feature macros instead.
///
/// Some useful information can be found here:
///
/// - http://wiki.apache.org/stdcxx/C++0xCompilerSupport
/// - http://clang.llvm.org/docs/LanguageExtensions.html
/// - http://gcc.gnu.org/projects/cxx0x.html
///
/// For MSVC, the values of the version macro are:
///
///     _MSC_VER = 1700 for Visual Studio 2012 / 11.0
///     _MSC_VER = 1600 for Visual Studio 2010 / 10.0
///     _MSC_VER = 1500 for Visual Studio 2008 /  9.0
///     _MSC_VER < 1500 for ancient, unsupported versions
///
/// For unsupported compilers, the preferable approach is to add specific
/// feature tests into the build system (e.g. Autoconf, CMake) and then define
/// the appropriate `HAVE_`... macros.


// Defined if *all* C++11 features are supported.
#if __cplusplus >= 201103L \
    && !defined(__clang__) \
    && !defined(__GNUC__)
#   undef  HAVE_CXX11
#   define HAVE_CXX11 1
#endif

// Local macros to reduce typing.  They should be #undef'ed afterwards.
#ifndef __has_feature
#   define __has_feature(x) 0
#endif
#define clang_feature(feature) (defined(__clang__) && __has_feature(feature))
#define gcc_version(major, minor)                                       \
    (defined(__GXX_EXPERIMENTAL_CXX0X__)                                \
     && (__GNUC__ == major && __GNUC_MINOR__ >= minor || __GNUC__ > major))
#define glib_date(date)                                                 \
    (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GLIBCXX__ >= date)

// ---------------------------------------------------------------------------
//
// Language features
// =================

// Defined if type inference (`auto`) is available.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_auto_type)             \
    || gcc_version(4, 4)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_AUTO
#   define HAVE_AUTO 1
#endif

// Defined if constant expressions (`constexpr`) are supported.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_constexpr)             \
    || gcc_version(4, 6)
#   undef  HAVE_CONSTEXPR
#   define HAVE_CONSTEXPR 1
#else
#   undef  constexpr
#   define constexpr
#endif
#if defined(HAVE_CXX1Y)                         \
    || clang_feature(cxx_relaxed_constexpr)
#   define FZ_CONSTEXPR_1Y constexpr
#else
#   define FZ_CONSTEXPR_1Y
#endif

// Defined if type inspection (`decltype`) is supported.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_decltype)              \
    || gcc_version(4, 3)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_DECLTYPE
#   define HAVE_DECLTYPE 1
#else
#   undef  constexpr
#   define constexpr
#endif

// Defined if function templates can have default arguments.
#if defined(HAVE_CXX11)                                         \
    || gcc_version(4, 3)                                        \
    || clang_feature(cxx_default_function_template_args)
#   undef  HAVE_DEFAULT_FUNC_TEMPLATE_ARG
#   define HAVE_DEFAULT_FUNC_TEMPLATE_ARG 1
#endif

// Defined if compile-time exception specifications (`noexcept`) are
// supported.
#if defined(HAVE_CXX11)                         \
    || gcc_version(4, 6)                        \
    || clang_feature(cxx_noexcept)
#   undef  HAVE_NOEXCEPT
#   define HAVE_NOEXCEPT 1
#else
#   undef  noexcept
#   define noexcept
#endif

// Defined if range-based `for` loops are supported.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_range_for)             \
    || gcc_version(4, 6)                        \
    || _MSC_VER >= 1700
#   undef  HAVE_RANGE_FOR
#   define HAVE_RANGE_FOR 1
#endif

// Defined if rvalue references are supported.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_rvalue_references)     \
    || gcc_version(4, 3)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_RVALUE
#   define HAVE_RVALUE 1
#endif

// Defined if static assertions (`static_assert`) are supported.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_static_assert)         \
    || gcc_version(4, 3)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_STATIC_ASSERT
#   define HAVE_STATIC_ASSERT 1
#endif

// Defined if template aliases are supported.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_alias_templates)       \
    || gcc_version(4, 7)
#   undef  HAVE_TEMPLATE_ALIAS
#   define HAVE_TEMPLATE_ALIAS 1
#endif

// Defined if the trailing return type syntax is supported.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_trailing_return)       \
    || gcc_version(4, 4)                        \
    || _MSC_VER >= 1600
#   undef  HAVE_TRAILING_RETURN
#   define HAVE_TRAILING_RETURN 1
#endif

// Defined if variadic templates are supported.
#if defined(HAVE_CXX11)                         \
    || clang_feature(cxx_variadic_templates)    \
    || gcc_version(4, 3)                        \
    || _MSC_VER >= 1700
#   undef  HAVE_VARIADIC_TEMPLATE
#   define HAVE_VARIADIC_TEMPLATE 1
#endif

// Defined if the `long long` integer type is available (since it's almost
// always the case, this is defined by default).  To suppress this definition,
// define `NO_LONG_LONG`.
#if !defined(HAVE_LONG_LONG) and !defined(NO_LONG_LONG)
#   define HAVE_LONG_LONG 1
#endif

// ---------------------------------------------------------------------------
//
// Library features
// ================

// Defined if `std::addressof` is available.
#if defined(HAVE_CXX11)
#   undef  HAVE_ADDRESSOF
#   define HAVE_ADDRESSOF 1
#endif

// Defined if the `<cstdint>` library is available.
#if defined(HAVE_CXX11)                         \
    || defined(__GXX_EXPERIMENTAL_CXX0X__)      \
    || _MSC_VER >= 1700
#   undef  HAVE_CSTDINT
#   define HAVE_CSTDINT 1
#endif

// Defined if `to_string` is available.
#if defined(HAVE_CXX11)                         \
    || gcc_version(4, 8)
#   define HAVE_STRING_HELPERS
#endif

// Defined if the `<type_traits>` library is available.
#if defined(HAVE_CXX11)                         \
    || gcc_version(4, 2)                        \
    || _MSC_VER >= 1500
#   undef  HAVE_TYPE_TRAITS
#   define HAVE_TYPE_TRAITS 1
#endif

// ---------------------------------------------------------------------------

// Remove the local macros
#undef clang_feature
#undef gcc_version
#undef glib_date

// For documentation purposes
#ifdef FZ_DOC_ONLY
#define HAVE_UINT32
#define HAVE_UINT64
#endif

// ---------------------------------------------------------------------------
//
// Integer types
// =============
//
// Determine some commonly used integer types.  Prefer <cstdint> or <stdint.h>
// if available; otherwise, figure it out the hard way.
//
#if defined(__GNUC__) || defined(__clang__)
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

namespace fz {

/// @addtogroup FzCxx11
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

/// @}

}

// ---------------------------------------------------------------------------
//
// Static assertion macro
// ======================

#if !defined(HAVE_STATIC_ASSERT) && !defined(static_assert)
#   define FZ_STATIC_ASSERT_MSG(line) FZ_STATIC_ASSERT_MSG2(line)
#   define FZ_STATIC_ASSERT_MSG2(line) static_assertion_failed_at_line_ ## line
//
/// @addtogroup FzUtility
/// @{
//
/// @def static_assert(expression, message)
///
/// Performs a static assertion of the expression.
///
/// Note: when template arguments are present, it may be necessary to protect
/// the `expression` with parentheses if the preprocessor interprets it
/// incorrectly (since it does not treat angle brackets as delimiters).
//
/// @}
//
#   define static_assert(expression, message)                           \
        enum { FZ_STATIC_ASSERT_MSG(__LINE__) = 1 / (int) (expression) }
#endif

// ---------------------------------------------------------------------------
//
// Type traits
// ===========

#ifdef HAVE_TYPE_TRAITS
#   include <type_traits>
#endif

/// Namespace `fz`.
namespace fz {

/// @defgroup FzUtility Utilities
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
struct match_cv                      { typedef T type; };
template<class T, class U>
struct match_cv<T, const U>          { typedef const T type; };
template<class T, class U>
struct match_cv<T, volatile U>       { typedef volatile T type; };
template<class T, class U>
struct match_cv<T, const volatile U> { typedef const volatile T type; };

/// @}

/// @addtogroup FzCxx11
/// @{

#ifdef HAVE_TYPE_TRAITS
using std::add_const;
using std::add_cv;
using std::add_lvalue_reference;
using std::add_pointer;
using std::add_rvalue_reference;
using std::add_volatile;
using std::common_type;
using std::conditional;
using std::decay;
using std::enable_if;
using std::extent;
using std::false_type;
using std::integral_constant;
using std::is_abstract;
using std::is_array;
using std::is_base_of;
using std::is_const;
using std::is_convertible;
using std::is_function;
using std::is_lvalue_reference;
using std::is_pointer;
using std::is_reference;
using std::is_rvalue_reference;
using std::is_same;
using std::is_void;
using std::is_volatile;
using std::make_signed;
using std::make_unsigned;
using std::rank;
using std::remove_all_extents;
using std::remove_const;
using std::remove_cv;
using std::remove_extent;
using std::remove_pointer;
using std::remove_reference;
using std::remove_volatile;
using std::result_of;
using std::true_type;
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
typedef integral_constant<bool, true>  true_type;

/// An alias to `integral_constant` with a `value` of `false`.
typedef integral_constant<bool, false> false_type;

/// Returns whether the type is `const`-qualified.
template<class T> struct is_const                : false_type {};
template<class T> struct is_const<const T>       : true_type {};

/// Returns whether the type is `volatile`-qualified.
template<class T> struct is_volatile             : false_type {};
template<class T> struct is_volatile<volatile T> : true_type {};

/// Adds the `const` qualifier to the type.
template<class T> struct add_const             { typedef const T type; };

/// Adds the `volatile` qualifier to the type.
template<class T> struct add_volatile       { typedef volatile T type; };

/// Adds both the `const` and `volatile` qualifiers to the type.
template<class T> struct add_cv       { typedef const volatile T type; };

/// Removes the `const` qualifier from the type.
template<class T> struct remove_const                { typedef T type; };
template<class T> struct remove_const<const T>       { typedef T type; };

/// Removes the `volatile` qualifier from the type.
template<class T> struct remove_volatile             { typedef T type; };
template<class T> struct remove_volatile<volatile T> { typedef T type; };

/// Removes both the `const` and `volatile` qualifiers from the type.
template<class T>
struct remove_cv {
    typedef typename remove_volatile<
        typename remove_const<T>::type
    >::type type;
};

/// Returns whether the types are the same.
template<class, class> struct is_same       : false_type {};
template<class T>      struct is_same<T, T> : true_type {};

/// Returns whether the type is a void type.
template<class T>
struct is_void
    : integral_constant<
        bool,
        is_same<
            void,
            typename remove_cv<T>::type
        >::value
    > {};

/// Returns whether the type is an lvalue or rvalue reference.
template<class T> struct is_lvalue_reference      : false_type {};
template<class T> struct is_lvalue_reference<T&>  : true_type {};

/// Returns whether the type is an rvalue reference.
template<class T> struct is_rvalue_reference      : false_type {};
#ifdef HAVE_RVALUE
template<class T> struct is_rvalue_reference<T&&> : true_type {};
#endif

/// Returns whether the type is an lvalue or rvalue reference.
template<class T> struct is_reference             : false_type {};
template<class T> struct is_reference<T&>         : true_type {};
#ifdef HAVE_RVALUE
template<class T> struct is_reference<T&&>        : true_type {};
#endif

namespace _priv {
template<class T, bool = is_void<T>::value>
struct add_lvalue_reference           { typedef T&  type; };
template<class T>
struct add_lvalue_reference<T, true>  { typedef T   type; };
}
/// Converts the type into an lvalue reference.
template<class T>
struct add_lvalue_reference : _priv::add_lvalue_reference<T> {};

namespace _priv {
template<class T, bool = is_void<T>::value>
struct add_rvalue_reference           { typedef T   type; };
#ifdef HAVE_RVALUE
template<class T>
struct add_rvalue_reference<T, false> { typedef T&& type; };
#endif
}
/// Converts the type into an rvalue reference, unless `T` is an lvalue
/// reference, in which case the type is returned unchanged.  If rvalue
/// references are not supported, the type is returned unchanged.
template<class T>
struct add_rvalue_reference : _priv::add_rvalue_reference<T> {};

/// Converts to a non-reference type.
template<class T> struct remove_reference      { typedef T type; };
template<class T> struct remove_reference<T&>  { typedef T type; };
#ifdef HAVE_RVALUE
template<class T> struct remove_reference<T&&> { typedef T type; };
#endif

#endif // HAVE_TYPE_TRAITS

/// @}

/// @addtogroup FzUtility
/// @{

// ---------------------------------------------------------------------------
//
// Macros
// ======

/// Used to remove parentheses around a type expression: when a type `T` is
/// passed in as a function type of the form `void(T)`, a typedef named `type`
/// is provided to recover the type `T`.
template<class>   struct unparenthesize_type;
template<class T> struct unparenthesize_type<void(T)> { typedef T type;    };
template<>        struct unparenthesize_type<void()>  { typedef void type; };

/// @def FZ_UNPARENS(type_expr)
///
/// Removes the parentheses around a type expression.
#ifndef FZ_DOC_ONLY
#   define FZ_UNPARENS(type_expr)                                       \
        typename ::fz::unparenthesize_type<void(type_expr)>::type
#else
#   define FZ_UNPARENS(type_expr) auto
#endif

/// @def FZ_VALID_VAL(expr)
///
/// Constructs an unevaluated context for the given expression to test the its
/// validity.  Returns the `return_value` if `value_expr` is valid; causes a
/// compiler error otherwise.  The result can be used to perform SFINAE tests.
#ifndef FZ_DOC_ONLY
#   define FZ_VALID_VAL(value_expr, return_value)               \
        (sizeof(value_expr, 0) ? return_value : return_value)
#else
#   define FZ_VALID_VAL(value_expr, return_value) return_value
#endif

/// @def FZ_VALID_TYPE(type_expr)
///
/// Returns the `return_type` if `type_expr` is a valid type; causes a
/// compiler error otherwise.  The result can be used to perform SFINAE tests.
#ifndef FZ_DOC_ONLY
#   define FZ_VALID_TYPE(type_expr, return_type)                        \
        typename ::fz::enable_if<                                       \
            FZ_VALID_VAL(fz::declval<FZ_UNPARENS(type_expr)>(), true),  \
            FZ_UNPARENS(return_type)                                    \
        >::type
#else
#   define FZ_VALID_TYPE(type_expr, return_type) return_type
#endif

/// @def FZ_DECLTYPE(expr, fallback_type)
///
/// Equivalent to `decltype(expr)` if available; otherwise, defaults to the
/// `fallback_type` provided that the expression is valid (the macro uses an
/// `enable_if` check to do this).
#if defined(HAVE_DECLTYPE) || defined(FZ_DOC_ONLY)
#   define FZ_DECLTYPE(expr, fallback_type) decltype(expr)
#else
#   define FZ_DECLTYPE(expr, fallback_type)     \
        typename ::fz::enable_if<               \
            FZ_VALID_VAL(expr, true),           \
            FZ_UNPARENS(fallback_type)          \
        >::type
#endif

// Doxygen does not handle decltype properly so it must be hidden
#ifdef FZ_DOC_ONLY
#   define decltype(expr) auto
#endif

/// @def FZ_ENABLE_IF(condition, type_expr)
///
/// Convenience macro for performing `enable_if` tests.
#ifndef FZ_DOC_ONLY
#   define FZ_ENABLE_IF(condition, type_expr)                                \
        typename ::fz::enable_if<(condition), FZ_UNPARENS(type_expr)>::type
#else
#   define FZ_ENABLE_IF(condition, type_expr) type_expr
#endif

/// @def FZ_ENABLE_IF_P(condition)
///
/// Convenience macro for performing `enable_if` tests in the parameter list.
#ifndef DOC_ONLY
#   define FZ_ENABLE_IF_P(condition)                    \
        , typename ::fz::enable_if<(condition)>::type* = 0
#else
#   define FZ_ENABLE_IF_P(condition)
#endif

/// @}

/// @addtogroup FzCxx11
/// @{

// ---------------------------------------------------------------------------
//
// Rvalue helper functions
// =======================

#ifdef HAVE_RVALUE_HELPERS
using std::declval;
using std::forward;
using std::move;
#else

/// Converts a type into an unevaluated operand.
template<class T>
typename add_rvalue_reference<T>::type declval() noexcept;

#if defined(HAVE_RVALUE) || defined(FZ_DOC_ONLY)
/// Perfectly forwards the given argument.
template<class T> inline constexpr
T&& forward(typename remove_reference<T>::type&& t) noexcept {
    static_assert(!is_lvalue_reference<T>::value,
                  "template argument T cannot be an lvalue reference");
    return static_cast<T&&>(t);
}
template<class T> inline constexpr
T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

/// Converts an rvalue reference into an xvalue.
template<class T> inline FZ_CONSTEXPR_1Y
typename remove_reference<T>::type&& move(T&& t) noexcept {
    static_cast<typename remove_reference<T>::type&&>(t);
}
#else // HAVE_RVALUE
/// Forwards the given argument (fallback version without rvalue support).
template<class T> inline constexpr
T& forward(T& t) noexcept {
    return t;
}
template<class T> inline constexpr
const T& forward(const T& t) noexcept {
    return t;
}
#endif // HAVE_RVALUE
#endif // HAVE_RVALUE_HELPERS

// ---------------------------------------------------------------------------
//
// Type traits (continued)
// ======================

#ifndef HAVE_TYPE_TRAITS

/// Returns one of the types based on a boolean condition.
template<bool Condition, class TrueT, class FalseT>
struct conditional                      { typedef FalseT type; };
template<class TrueT, class FalseT>
struct conditional<true, TrueT, FalseT> { typedef  TrueT type; };

/// Contains a `type` member equal to `T` if `Condition` is `true`.
template<bool Condition, class T = void> struct enable_if {};
template<class T> struct enable_if<true, T> { typedef T type; };

namespace _priv {
template<class T> struct make_signed {};
template<> struct make_signed<unsigned char>      { typedef signed char type; };
template<> struct make_signed<unsigned short>     { typedef       short type; };
template<> struct make_signed<unsigned>           { typedef         int type; };
template<> struct make_signed<unsigned long>      { typedef        long type; };
#ifdef HAVE_LONG_LONG
template<> struct make_signed<unsigned long long> { typedef   long long type; };
#endif
}
/// Converts an integral type to a signed type (only works on fundamental
/// types, excluding `char`).
template<class T> struct make_signed {
    typedef typename match_cv<
        typename _priv::make_signed<
            typename remove_cv<T>::type
        >::type,
        T
    >::type type;
};

namespace _priv {
template<class T> struct make_unsigned {};
template<> struct make_unsigned<signed char> { typedef unsigned    char type; };
template<> struct make_unsigned<short>       { typedef unsigned   short type; };
template<> struct make_unsigned<int>         { typedef unsigned         type; };
template<> struct make_unsigned<long>        { typedef unsigned    long type; };
#ifdef HAVE_LONG_LONG
template<> struct make_unsigned<long long> { typedef unsigned long long type; };
#endif
}
/// Converts an integral type to a unsigned type (only works on fundamental
/// types, excluding `char`).
template<class T> struct make_unsigned {
    typedef typename match_cv<
        typename _priv::make_unsigned<
            typename remove_cv<T>::type
        >::type,
        T
    >::type type;
};

namespace _priv {
template<class T> struct is_integral       : false_type {};
template<> struct is_integral<wchar_t>     : true_type {};
template<> struct is_integral<bool>        : true_type {};
template<> struct is_integral<char>        : true_type {};
template<> struct is_integral<signed char> : true_type {};
template<> struct is_integral<short>       : true_type {};
template<> struct is_integral<int>         : true_type {};
template<> struct is_integral<long>        : true_type {};
#ifdef HAVE_LONG_LONG
template<> struct is_integral<long long>   : true_type {};
#endif
template<class T, class = void> struct is_integral2 : is_integral<T> {};
template<class T> struct is_integral2<T, typename conditional<
        true, void, typename make_signed<T>::type>::type>
    :   is_integral<typename make_signed<T>::type> {};
}
/// Returns whether the type is an integral type (only works on fundamental
/// types).
template<class T> struct is_integral
    : _priv::is_integral2<typename remove_cv<T>::type> {};

namespace _priv {
template<class T> struct is_floating_point       : false_type {};
template<> struct is_floating_point<float>       : true_type {};
template<> struct is_floating_point<double>      : true_type {};
template<> struct is_floating_point<long double> : true_type {};
}
/// Returns whether the type is a floating-point type (only works on
/// fundamental types).
template<class T> struct is_floating_point
    : _priv::is_floating_point<typename remove_cv<T>::type> {};

/// Returns whether the type is an integral or floating-point type.
template<class T>
struct is_arithmetic
    : integral_constant<bool, is_integral<T>::value ||
                              is_floating_point<T>::value> {};

namespace _priv {
template<class Base, class T> struct is_base_of {
    struct conv { operator Base*() const; operator T*(); };
    template<class Int>
    static char (&check(T*, Int));
    static char (&check(Base*, int))[2];
    static const bool value = sizeof(check(conv(), 0)) == 1;
};
template<class T> struct is_base_of<T, T> { static const bool value = true; };
}
/// Checks if a type is derived from another type.
template<class Base, class Derived>
struct is_base_of
    : integral_constant<bool, _priv::is_base_of<Base, Derived>::value> {};

namespace _priv {
template<class T>
struct is_abstract {
    template<class U>
    static char (&check(...));
    template<class U>
    static char (&check(U (*)[1]))[2];
    static_assert(sizeof(T), "T must be a complete type.");
    static const unsigned value = sizeof(check<T>(0)) == 1;
};
}
/// Checks if a type is an abstract type.
template<class T>
struct is_abstract
    : integral_constant<bool, _priv::is_abstract<T>::value> {};

namespace _priv {
template<class From, class To, class = void>
struct is_convertible {
    struct any {
        template<class T> any(const volatile T&);
        template<class T> any(T&);
    };
    static char (&check(...))[2];
    static char (&check(To, int))[1];
#ifdef HAVE_RVALUE
    static From&& from();
#else
    static From from();
#endif
    static const bool value = sizeof(check(from(), 0)) == 1;
};
template<class From, class To>
struct is_convertible<From, To,
    typename enable_if<
        is_void<typename remove_cv<From>::type>::value ||
        is_void<typename remove_cv<To>::type>::value
    >::type> : is_same<typename remove_cv<From>::type,
                       typename remove_cv<To>::type> {};
// Not strictly required, but this is used to suppress warnings about
// arithmetic type conversions in MSVC.
template<class From, class To>
struct is_convertible<From, To,
    typename enable_if<
        is_arithmetic<From>::value ||
        is_arithmetic<To>::value
    >::type> : true_type {};
}
/// Checks if the type `From` can be converted to `To`.
template<class From, class To>
struct is_convertible : _priv::is_convertible<From, To> {};

namespace _priv {
#ifdef HAVE_DECLTYPE
template<class T, class U>
struct common_type {
    typedef decltype(true ? declval<T>() : declval<U>()) type;
};
#else
template<class, class, class = void>
struct common_type {}; // Currently not defined
template<class T, class U>
struct common_type<T, U, typename enable_if<is_same<T, U>::value>::type> {
    typedef T type;
};
#endif
}
/// Returns a common type among the given types.  Without support for variadic
/// templates, this template can only support 2 template arguments.  Without
/// `decltype` support, the common type is only defined if the types are thes
/// same).
#ifdef HAVE_VARIADIC_TEMPLATE
template<class...>
struct common_type {};
template<class T>
struct common_type<T> { typedef T type; };
template<class T, class U>
struct common_type<T, U> : _priv::common_type<T, U> {};
template<class T, class U, class... V>
struct common_type<T, U, V...>
    :  common_type<typename common_type<T, U>::type, V...> {};
#else
template<class T, class U>
struct common_type : _priv::common_type<T, U> {};
#endif

namespace _priv {
template<class, class = void> struct result_of {};
#ifdef HAVE_DECLTYPE
#ifdef HAVE_VARIADIC_TEMPLATE
template<class F, class... T> struct result_of<F(T...),
    FZ_VALID_TYPE(decltype(declval<F>()(declval<T>()...)), void)> {
    typedef       decltype(declval<F>()(declval<T>()...)) type;
};
#else
template<class F>
struct result_of<F(),
    FZ_VALID_TYPE(decltype(declval<F>()()), void)> {
    typedef       decltype(declval<F>()()) type;
};
template<class F, class T>
struct result_of<F(T),
    FZ_VALID_TYPE(decltype(declval<F>()(declval<T>())), void)> {
    typedef       decltype(declval<F>()(declval<T>())) type;
};
template<class F, class T, class U>
struct result_of<F(T, U),
    FZ_VALID_TYPE(decltype(declval<F>()(declval<T>(), declval<U>())), void)> {
    typedef       decltype(declval<F>()(declval<T>(), declval<U>())) type;
};
template<class F, class T, class U, class V>
struct result_of<F(T, U, V),
    FZ_VALID_TYPE(decltype(declval<F>()(declval<T>(), declval<U>(),
                                        declval<V>())), void)> {
    typedef       decltype(declval<F>()(declval<T>(), declval<U>(),
                                        declval<V>())) type;
};
template<class F, class T, class U, class V, class W>
struct result_of<F(T, U, V, W),
    FZ_VALID_TYPE(decltype(declval<F>()(declval<T>(), declval<U>(),
                                        declval<V>(), declval<W>())), void)> {
    typedef       decltype(declval<F>()(declval<T>(), declval<U>(),
                                        declval<V>(), declval<W>())) type;
};
template<class F, class T, class U, class V, class W, class X>
struct result_of<F(T, U, V, W, X),
    FZ_VALID_TYPE(decltype(declval<F>()(declval<T>(), declval<U>(),
                                        declval<V>(), declval<W>(),
                                        declval<X>())), void)> {
    typedef       decltype(declval<F>()(declval<T>(), declval<U>(),
                                        declval<V>(), declval<W>(),
                                        declval<X>())) type;
};
#endif
#else
#ifdef HAVE_VARIADIC_TEMPLATE
template<class F, class... T>
struct result_of<F(T...),
    FZ_VALID_TYPE(typename F::result_type, void)> {
    typedef typename F::result_type type;
};
#else
template<class F>
struct result_of<F(),
    FZ_VALID_TYPE(typename F::result_type, void)> {
    typedef typename F::result_type type;
};
template<class F, class T>
struct result_of<F(T),
    FZ_VALID_TYPE(typename F::result_type, void)> {
    typedef typename F::result_type type;
};
template<class F, class T, class U>
struct result_of<F(T, U),
    FZ_VALID_TYPE(typename F::result_type, void)> {
    typedef typename F::result_type type;
};
template<class F, class T, class U, class V>
struct result_of<F(T, U, V),
    FZ_VALID_TYPE(typename F::result_type, void)> {
    typedef typename F::result_type type;
};
template<class F, class T, class U, class V, class W>
struct result_of<F(T, U, V, W),
    FZ_VALID_TYPE(typename F::result_type, void)> {
    typedef typename F::result_type type;
};
template<class F, class T, class U, class V, class W, class X>
struct result_of<F(T, U, V, W, X),
    FZ_VALID_TYPE(typename F::result_type, void)> {
    typedef typename F::result_type type;
};
#endif
#endif
}
/// Deduces the return type of a function call (up to 5 arguments if variadic
/// templates are not supported).  If `decltype` is not available, it will use
/// the `result_type` member of the function.
template<class T>
struct result_of : _priv::result_of<T> {};

/// Returns the number of dimensions in the array type.
template<class T>                struct rank
    : integral_constant<std::size_t, 0> {};
template<class T>                struct rank<T[]>
    : integral_constant<std::size_t, rank<T>::value + 1> {};
template<class T, std::size_t N> struct rank<T[N]>
    : integral_constant<std::size_t, rank<T>::value + 1> {};

/// Returns whether the type is an array type.
template<class T>                struct is_array       : false_type {};
template<class T>                struct is_array<T[]>  : true_type {};
template<class T, std::size_t N> struct is_array<T[N]> : true_type {};

/// Obtains the number of elements along the `N`-th dimension (or zero if
/// unknown or outside bounds).
template<class T, unsigned N = 0>            struct extent
    : integral_constant<std::size_t, 0> {};
template<class T>                            struct extent<T[], 0>
    : integral_constant<std::size_t, 0> {};
template<class T, unsigned N>                struct extent<T[], N>
    : integral_constant<std::size_t, extent<T, N - 1>::value> {};
template<class T, std::size_t N>             struct extent<T[N], 0>
    : integral_constant<std::size_t, N> {};
template<class T, std::size_t I, unsigned N> struct extent<T[I], N>
    : integral_constant<std::size_t, extent<T, N - 1>::value> {};

/// Removes the first dimension of an array type.
template<class T>
struct remove_extent       { typedef T type; };
template<class T>
struct remove_extent<T[]>  { typedef T type; };
template<class T, std::size_t N>
struct remove_extent<T[N]> { typedef T type; };

/// Removes all the dimensions of an array type.
template<class T>
struct remove_all_extents  { typedef T type; };
template<class T>
struct remove_all_extents<T[]>  : remove_all_extents<T> {};
template<class T, std::size_t N>
struct remove_all_extents<T[N]> : remove_all_extents<T> {};

/// Returns whether the the type is a function type (up to 5 arguments if
/// variadic templates are not supported).
template<class>
struct is_function                   : false_type {};
#ifdef HAVE_VARIADIC_TEMPLATE
template<class R, class... Args>
struct is_function<R(Args...)>       : true_type {};
#else
template<class R>
struct is_function<R()>              : true_type {};
template<class R, class T>
struct is_function<R(T)>             : true_type {};
template<class R, class T, class U>
struct is_function<R(T, U)>          : true_type {};
template<class R, class T, class U, class V>
struct is_function<R(T, U, V)>       : true_type {};
template<class R, class T, class U, class V, class W>
struct is_function<R(T, U, V, W)>    : true_type {};
template<class R, class T, class U, class V, class W, class X>
struct is_function<R(T, U, V, W, X)> : true_type {};
#endif

/// Converts into a pointer type.
template<class T>
struct add_pointer { typedef typename remove_reference<T>::type* type; };

namespace _priv {
template<class T> struct is_pointer : false_type {};
template<class T> struct is_pointer<T*> : true_type {};
}
/// Returns whether the type is an object pointer type.
template<class T>
struct is_pointer : _priv::is_pointer<typename remove_cv<T>::type> {};

namespace _priv {
template<class T> struct remove_pointer     { typedef T type; };
template<class T> struct remove_pointer<T*> { typedef T type; };
}
/// Converts to a non-pointer type.
template<class T>
struct remove_pointer : _priv::remove_pointer<typename remove_cv<T>::type> {};

namespace _priv {
template<class T, class U = typename remove_reference<T>::type>
struct decay {
    typedef typename conditional<
        is_array<U>::value,
        typename remove_extent<U>::type*,
        typename conditional<
            is_function<U>::value,
            typename add_pointer<U>::type,
            typename remove_cv<U>::type
        >::type
    >::type type;
};
}
/// Performs lvalue transformations and removes cv-qualifiers.
template<class T>
struct decay : _priv::decay<T> {};

#endif // HAVE_TYPE_TRAITS

// ---------------------------------------------------------------------------
//
// Utility functions
// =================

#ifdef HAVE_ADDRESSOF
using std::addressof;
#else
/// Obtains the address of an object even if `operator&` is overloaded.
template<class T>
inline T* addressof(T& x) {
    return reinterpret_cast<T*>(
        &const_cast<char&>(reinterpret_cast<const volatile char&>(x)));
}
#endif // HAVE_ADDRESSOF

// ---------------------------------------------------------------------------
//
// String helper functions
// =======================

#ifdef HAVE_STRING_HELPERS
using std::to_string;
#endif

/// Constructs a string representation of an object using the stream insertion
/// operator (`<<`).
template<class T>
inline std::string to_string(const T& x) {
    std::ostringstream stream;
    stream << x;
    return stream.str();
}

/// @}

// ---------------------------------------------------------------------------
//
// Miscellaneous
// =============

/// @addtogroup FzUtility
/// @{

/// Constructs a string representation of an object using the stream insertion
/// operator (`<<`).
template<class T>
inline std::string to_string(const T& x); // Defined earlier

namespace _priv {
template<class, class = void> struct has_const_iterator : false_type {};
template<class T>             struct has_const_iterator<T,
    FZ_VALID_TYPE(typename T::const_iterator, void)>    : true_type  {};
template<class, class = void> struct has_iterator       : false_type {};
template<class T>             struct has_iterator<T,
    FZ_VALID_TYPE(typename T::iterator, void)>          : true_type  {};
template<class, class = void> struct get_iterator2 {};
template<class T>             struct get_iterator2<T,
    FZ_ENABLE_IF(is_array<T>::value, void)> {
    typedef typename remove_extent<T>::type type;
};
template<class T>             struct get_iterator2<T,
    FZ_ENABLE_IF(has_const_iterator<T>::value &&
                 has_iterator<T>::value, void)> {
    typedef typename conditional<
        is_const<T>::value,
        typename T::const_iterator,
        typename T::iterator
    >::type type;
};
template<class T>             struct get_iterator2<T,
    FZ_ENABLE_IF(has_const_iterator<T>::value &&
                 !has_iterator<T>::value, void)> {
    typedef typename T::const_iterator type;
};
template<class T>             struct get_iterator2<T,
    FZ_ENABLE_IF(!has_const_iterator<T>::value &&
                 has_iterator<T>::value, void)> {
    typedef typename T::iterator type;
};
template<class It>            struct get_iterator2<
    std::pair<It, It>, FZ_VALID_TYPE(typename It::iterator_category, void)> {
    typedef It type;
};
template<class, class = void> struct get_iterator {};
template<class T>             struct get_iterator<T,
    FZ_VALID_TYPE(
        (FZ_DECLTYPE(
            begin(declval<T>()),
            typename get_iterator2<T>::type
        )),
        void
    )> {
    typedef FZ_DECLTYPE(
        begin(declval<T>()),
        typename get_iterator2<T>::type
    ) type;
};
}
/// Obtains the default iterator type of a container or array.
template<class T>
struct get_iterator : _priv::get_iterator<T> {};

/// @}

// ---------------------------------------------------------------------------
//
// Iterator helper functions
// =========================

/// @addtogroup FzCxx11
/// @{

#ifdef HAVE_BEGIN_END
using std::begin;
using std::end;
#else

/// Returns an iterator to the beginning of a container.
template<class C> inline
FZ_DECLTYPE(declval<C&>().begin(),       typename get_iterator<C&>::type)
begin(C& c)          { return c.begin(); }

/// Returns an iterator to the beginning of a container.
template<class C> inline
FZ_DECLTYPE(declval<const C&>().begin(), typename get_iterator<const C&>::type)
begin(const C& c)    { return c.begin(); }

/// Returns an iterator to the beginning of an array.
template<class T, std::size_t N> inline T*
begin(T (&array)[N]) { return array;     }

/// Returns a const-qualified iterator to the beginning of a container.
template<class C> inline
FZ_DECLTYPE(begin(declval<const C&>()), typename get_iterator<const C&>::type)
cbegin(const C& c)   { return begin(c);  }

/// Returns an iterator to the end of a container.
template<class C> inline
FZ_DECLTYPE(declval<C&>().end(),        typename get_iterator<C&>::type)
end(C& c)            { return c.end();   }

/// Returns an iterator to the end of a container.
template<class C> inline
FZ_DECLTYPE(declval<const C&>().end(),  typename get_iterator<const C&>::type)
end(const C& c)      { return c.end();   }

/// Returns an iterator to the end of an array.
template<class T, std::size_t N> inline T*
end(T (&array)[N])   { return array + N; }

/// Returns a const-qualified iterator to the end of a container.
template<class C> inline
FZ_DECLTYPE(end(declval<const C&>()),   typename get_iterator<const C&>::type)
cend(const C& c)     { return end(c);    }

#endif // HAVE_BEGIN_END

/// @}

} // namespace fz

namespace std {

/// @addtogroup FzCxx11
/// @{

/// Hash function declaration (no specializations provided).
template<class Key>
struct hash;

/// @}

/// @addtogroup FzUtility
/// @{

/// Returns the first item in an iterator pair.
template<class Iterator> inline
FZ_VALID_TYPE(
    typename std::iterator_traits<Iterator>::iterator_category,
    Iterator
) begin(const std::pair<Iterator, Iterator>& p) { return p.first;  }

/// Returns the second item in an iterator pair.
template<class Iterator> inline
FZ_VALID_TYPE(
    typename std::iterator_traits<Iterator>::iterator_category,
    Iterator
) end(const std::pair<Iterator, Iterator>& p)   { return p.second; }

/// @}

} // namespace std
#endif
