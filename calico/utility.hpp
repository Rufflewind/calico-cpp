#ifndef WNCVHCGXGYKJMVXUERTQ
#define WNCVHCGXGYKJMVXUERTQ
/** @mainpage

Introduction
============

The majority of this library is header-only, so for those one can simply
install them (all contained in the `./calico` subdirectory) at an appropriate
place and include the headers in the code.

The only exception is the [Windows entry point module](@ref winentry.h), which
must be compiled and linked with the rest of the program.

Start by examining the various [modules](modules.html) and
[headers](files.html).

*/
/// @file
///
/// Miscellaneous utility functions.
namespace cal {

// Hide the unnecessary clutter in the generated documentation.
#ifdef CALICO_DOC_ONLY
#  define typename
#  define CALICO_ALT(type_expr, alt_type_expr) alt_type_expr
#else
#  define CALICO_ALT(type_expr, alt_type_expr) \
     typename unparenthesize_type<void(type_expr)>::type
#endif
#define CALICO_HIDE(type_expr) CALICO_ALT(type_expr, auto)

// Returns the `return_type` if `type_expr` is a valid type; causes an error
// otherwise.  Also hides this expression from doxygen.
#ifndef CALICO_VALID_TYPE
#  define CALICO_VALID_TYPE(type_expr, return_type)                         \
     CALICO_ALT(                                                            \
         (typename std::conditional<                                        \
              0,                                                            \
              CALICO_HIDE(type_expr),                                       \
              CALICO_HIDE(return_type)                                      \
          >::type),                                                         \
         return_type                                                        \
     )
#endif

// Returns the `return_type` if `condition` is true; causes an error
// otherwise.  Also hides this expression from doxygen.
#ifndef CALICO_ENABLE_IF
#  define CALICO_ENABLE_IF(condition, return_type)                          \
     CALICO_ALT(                                                            \
         (typename std::enable_if<                                          \
              (condition),                                                  \
              CALICO_HIDE(return_type)                                      \
          >::type),                                                         \
         return_type                                                        \
     )
#endif

// *DEPRECATE* These guards can be removed once `cxx11.hpp` has been retired.
#ifndef CALICO_HAVE_UNPARENTHESIZE_TYPE
#define CALICO_HAVE_UNPARENTHESIZE_TYPE
template<class> struct unparenthesize_type;
/// Used to remove parentheses around a type expression.
///
/// When a `TypeExpression` is passed in as a function type of the form
/// `void(TypeExpression)`, a member typedef is provided to recover the
/// `TypeExpression` without extra parentheses.  If the `TypeExpression` is
/// empty, the member typedef becomes simply `void`.
///
/// This is typically used in macros to remove extra parentheses that are
/// needed to prevent the preprocessor from breaking up type expressions at
/// commas.
template<class TypeExpression>
struct unparenthesize_type<void(TypeExpression)> {
    /// The original `TypeExpression`.  If the `TypeExpression` is empty, the
    /// member typedef becomes simply `void`.
    typedef TypeExpression type;
};
template<> struct unparenthesize_type<void()> { typedef void type; };
#endif

// *DEPRECATE* These guards can be removed once `cxx11.hpp` has been retired.
#ifndef CALICO_HAVE_MATCH_CV
#define CALICO_HAVE_MATCH_CV
/// Sets the const-volatile qualifier of a type `T` to match the `Target`
/// type.
template<class T, class Target>
struct match_cv {
    /// A type related to `T` with the same const-volatile qualifiers as
    /// `Target`.
    typedef CALICO_HIDE(T) type;
};
template<class T, class U>
struct match_cv<T, const U>          { typedef const T type; };
template<class T, class U>
struct match_cv<T, volatile U>       { typedef volatile T type; };
template<class T, class U>
struct match_cv<T, const volatile U> { typedef const volatile T type; };
#endif

// *DEPRECATE* These guards can be removed once `cxx11.hpp` has been retired.
#ifndef CALICO_HAVE_VALID_CALL
#define CALICO_HAVE_VALID_CALL
namespace _priv {
template<class, class = void>
struct valid_call : std::false_type {};
template<class F, class... T>
struct valid_call<F(T...), typename std::enable_if<
    sizeof(std::declval<F>()(std::declval<T>()...), 0) ? 1 : 1
>::type> : std::true_type {};
}
/// Returns whether a function call is valid.
///
/// When instantiated with a template argument of the form `F(T...)`, its
/// `value` is `true` if the function object `F` can be called with arguments
/// of type `T...` and `false` otherwise.
template<class T> struct valid_call
#ifndef CALICO_DOC_ONLY
  : _priv::valid_call<T> {};
#else
{
    /// Whether the function object can be called with the given argument
    /// types.
    static const bool value;
};
#endif
#endif

namespace _priv {
// Need to introduce `std::begin` here to allow ADL to work properly.
using std::begin;
template<class, class = void> struct iterator_type {};
template<class T>             struct iterator_type<T,
    typename std::conditional<0,
          decltype(begin(std::declval<T>())),
    void>::type>
{ typedef decltype(begin(std::declval<T>())) type; };
}
/// Returns the iterator type of the given container-like type.
///
/// This is equivalent to `decltype(begin(std::declval<T>()))` with
/// `std::begin` present in the current scope.
template<class T> struct iterator_type
#ifndef CALICO_DOC_ONLY
  : _priv::iterator_type<T> {};
#else
{
    /// The iterator type of the given container-like type.
    typedef auto type;
};
#endif

}
#endif
