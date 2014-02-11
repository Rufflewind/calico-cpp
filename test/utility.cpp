#include <cassert>
#include <iostream>
#include <type_traits>
#include <calico/utility.hpp>
using namespace cal;

void foo(int i, double j, const char* k) {
    //std::cout << i << " " << j << " " << k << std::endl;
}

int main() {
    pack_params(foo)(std::make_tuple(-42, 0.5, "String"));

    assert((std::is_same<
        combine_tuples_t<std::tuple<double>, std::tuple<float> >,
        std::tuple<double, float>
    >::value));
    assert((std::is_same<
        combine_tuples_t<std::tuple<>, std::tuple<float, int> >,
        std::tuple<float, int>
    >::value));
    assert((std::is_same<
        ntuple_t<double, 0>,
        std::tuple<>
    >::value));
    assert((std::is_same<
        ntuple_t<double, 1>,
        std::tuple<double>
    >::value));
    assert((std::is_same<
        ntuple_t<double, 2>,
        std::tuple<double, double>
    >::value));
    assert((std::is_same<
        ntuple_t<double, 3>,
        std::tuple<double, double, double>
    >::value));
}
