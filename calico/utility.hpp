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
#include <tuple>
#include <ostream>
#include <type_traits>
/// Primary namespace.
namespace cal {

// Hide the unnecessary clutter in the generated documentation.
#ifdef CALICO_DOC_ONLY
#  define typename
#  define CALICO_ALT(type_expr, alt_type_expr) alt_type_expr
#else
#  define CALICO_ALT(type_expr, alt_type_expr) \
     ::cal::unparenthesize_type_t<void(type_expr)>
#endif
#define CALICO_HIDE(type_expr) CALICO_ALT(type_expr, auto)

// Returns the `return_type` if `type_expr` is a valid type; causes an error
// otherwise.  Also hides this expression from doxygen.
#ifndef CALICO_VALID_TYPE
#  define CALICO_VALID_TYPE(type_expr, return_type)                         \
     CALICO_ALT(                                                            \
         (typename ::std::conditional<                                      \
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
         (typename ::std::enable_if<                                        \
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

/// @see unparenthesize_type
template<class T>
using unparenthesize_type_t = typename unparenthesize_type<T>::type;

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

/// @see match_cv
template<class T, class Target>
using match_cv_t = typename match_cv<T, Target>::type;

// *DEPRECATE* These guards can be removed once `cxx11.hpp` has been retired.
#ifndef CALICO_HAVE_VALID_CALL
#define CALICO_HAVE_VALID_CALL
namespace _priv {
template<class, class = void>
struct valid_call : std::false_type {};
template<class F, class... T>
struct valid_call<F(T...), typename std::conditional<0,
    typename std::result_of<F(T...)>::type,
void>::type> : std::true_type {};
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

/// Combines several `tuple` types into a single `tuple` type.
template<class... Tuples>
struct combine_tuples {
#ifdef CALICO_DOC_ONLY
    /// A single `tuple` type formed by combining multiple `tuple` types.
    typedef std::tuple<..> type;
#endif
};
template<>
struct combine_tuples<> {
    typedef std::tuple<> type;
};
template<class... Ts>
struct combine_tuples<std::tuple<Ts...> > {
    typedef std::tuple<Ts...> type;
};
template<class... Ts, class... Us, class... Tuples>
struct combine_tuples<std::tuple<Ts...>, std::tuple<Us...>, Tuples...> {
    typedef typename combine_tuples<
        std::tuple<Ts..., Us...>,
        Tuples...
    >::type type;
};

/// @see combine_tuples
template<class... Tuples>
using combine_tuples_t = typename combine_tuples<Tuples...>::type;

/// Constructs a `tuple` type containing `N` objects of type `T`.
template<class T, std::size_t N>
struct ntuple {
#ifdef CALICO_DOC_ONLY
    /// A `tuple` type containing `N` objects of type `T`.
    typedef std::tuple<UNSPECIFIED> type;
#else
    typedef combine_tuples_t<
        std::tuple<T>,
        typename ntuple<T, N - 1>::type
    > type;
#endif
};
template<class T>
struct ntuple<T, 0> {
    typedef std::tuple<> type;
};

/// @see ntuple
template<class T, std::size_t N>
using ntuple_t = typename ntuple<T, N>::type;

/// A wrapper function type that allows a function to be called with a `tuple`
/// whose elements are unpacked as arguments.
template<class Function>
struct packed_params {
    packed_params(const Function& f) : _f(f) {}
    Function _f;
    template<std::size_t J, std::size_t... I>
    struct fold {
        template<class Tuple>
        static auto apply(const Function& f, const Tuple& x)
        -> decltype(fold<J - 1, J - 1, I...>::apply(f, x)) {
            return  fold<J - 1, J - 1, I...>::apply(f, x);
        }
    };
    template<std::size_t... I>
    struct fold<0, I...> {
        template<class Tuple>
        static auto apply(const Function& f, const Tuple& x)
        -> decltype(f(std::get<I>(x)...)) {
            return  f(std::get<I>(x)...);
        }
    };

    /// Calls the function with the elements of a `tuple` as arguments.
    template<class Tuple>
    auto operator()(const Tuple& x) const
#ifndef CALICO_DOC_ONLY
    -> decltype(fold<std::tuple_size<Tuple>::value>::apply(_f, x))
#endif
    {
        return  fold<std::tuple_size<Tuple>::value>::apply(_f, x);
    }
};

/// Constructs a wrapper function object that allows a function to be called
/// with a `tuple` whose elements are unpacked as arguments.
template<class Function> inline
#ifdef CALICO_DOC_ONLY
auto
#else
packed_params<typename std::decay<Function>::type>
#endif
pack_params(const Function& f) {
    return packed_params<typename std::decay<Function>::type>(f);
}

}

namespace std {
std::ostream& operator<<(std::ostream&, const std::tuple<>&);
template<class... Ts>
std::ostream& operator<<(std::ostream&, const std::tuple<Ts...>&);
}

namespace cal {
namespace _priv {
template<class T, std::size_t N1, std::size_t I = 0>
struct tuple_output_helper {
    static void apply(std::ostream& s, const T& x) {
        s << std::get<I>(x) << ", ";
        tuple_output_helper<T, N1, I + 1>::apply(s, x);
    }
};
template<class T, std::size_t N1>
struct tuple_output_helper<T, N1, N1> {
    static void apply(std::ostream& s, const T& x) {
        s << std::get<N1>(x);
    }
};
}
}

namespace std {

inline std::ostream& operator<<(std::ostream& s, const std::tuple<>&) {
    return s << "()";
}
/// Outputs a `tuple` in the format `(a, b, c, ...)`.
template<class... Ts> inline
std::ostream& operator<<(std::ostream& s, const std::tuple<Ts...>& x) {
    s        << "(";
    cal::_priv::tuple_output_helper<std::tuple<Ts...>, sizeof...(Ts) - 1>
              ::apply(s, x);
    return s << ")";
}

}
#endif
