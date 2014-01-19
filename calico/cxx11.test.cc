#undef _MSC_VER
//#define HAVE_STATIC_ASSERT
#include "cxx11.hh"
using namespace fz;

struct Abstract { virtual ~Abstract() = 0; };
struct Concrete {};
struct A {};
struct B {};
struct C : A {};


#ifdef HAVE_RVALUE
#define RVALUE(T) T&&
#else
#define RVALUE(T) T
#endif
namespace p {
template<class T, bool = is_void<T>::value>
struct add_rvalue_reference { typedef RVALUE(T) type; };
template<class T>
struct add_rvalue_reference<T, true> { typedef T type; };
}
/// Converts the type into an rvalue reference provided that `T` is not an
/// lvalue reference.
template<class T>
struct add_rv_ref {
    typedef typename p::add_rvalue_reference<T>::type type;
};

template<class T>
typename add_rv_ref<T>::type create();
template<class From, class To>
To test() { return create<From>(); }

#define convertible_test(From, To) \
    static_assert((is_convertible<From, To>::value), ""); \
    test<From, To>();
#define convertible_testN(From, To) \
    static_assert((!is_convertible<From, To>::value), "");

void test() {
    static_assert(is_abstract<Abstract>::value, "");
    static_assert(!is_abstract<Concrete>::value, "");
    convertible_test(int, float);
    convertible_test(float, int);
    convertible_test(int, unsigned);
    convertible_test(unsigned, int);
    test<long long, short>();
    convertible_testN(A, A&);
    //test<A, A&>();
    convertible_test(A&, A);
    convertible_test(A, A);
    convertible_test(C, A);
    convertible_testN(A, C);
    //test<A, C>();
    convertible_test(C, C);
    convertible_testN(A, void);
    //test<A, void>();
    convertible_testN(void, A);
    //test<void, A>();
    convertible_test(void, void);
    convertible_test(void, const volatile void);
    convertible_test(const volatile void, const void);
}

int main() { return 0; }
