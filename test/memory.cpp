#include <cassert>
#include <iostream>
#include <calico/memory.hpp>
using namespace cal;

int main() {

    double C_arr[] = {1, 2, 3, 4};

    vec<double> B_vec(500);
    auto B = B_vec.to_slice();

    B[0] = 150;
    assert(B[0] == 150);

    B.clear();
    for (auto& i : B)
        assert(i == 0);

    std::move(B_vec).as_unique_ptr();   // B is invalidated
    B = B_vec.to_slice();
    assert(B.size() == 0);

    B_vec.resize(50);                   // B is invalidated
    B = B_vec.to_slice();

    B.to_slice(20, 25).copy_from(slice_from_array(C_arr).to_const());
    {
        double B_cmp_arr[] = {0, 1, 2, 3, 4, 0, 0};
        auto B_cmp = slice_from_array(B_cmp_arr);
        assert(equal(B.to_slice(19, 19 + B_cmp.size()), B_cmp));
    }

    B.to_slice(21).copy_from(B.to_slice(20, 24).to_const());
    {
        double B_cmp_arr[] = {0, 1, 1, 2, 3, 4, 0};
        auto B_cmp = slice_from_array(B_cmp_arr);
        assert(equal(B.to_slice(19, 19 + B_cmp.size()), B_cmp));
    }

    B.to_slice(20).copy_from(B.to_slice(21, 25).to_const());
    {
        double B_cmp_arr[] = {0, 1, 2, 3, 4, 4, 0};
        auto B_cmp = slice_from_array(B_cmp_arr);
        assert(equal(B.to_slice(19, 19 + B_cmp.size()), B_cmp));
    }

    double A_arr[] = {1, 2, 3};
    auto A = slice_from_array(A_arr);

    assert(A.to_slice(1, 999)[1] == 3);

    A.deinitialize();
    A.initialize();

    assert(A.to_slice(2, 3)[0]     == 0);
    assert(A.to_slice(4, 3).size() == 0);

    return 0;
}
