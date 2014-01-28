#include <cassert>
#include <calico/lens.hpp>
using namespace cal;

// This does in fact optimize to `test_negating_lens2` with `g++ -O2` with
// `int`s.  With `double`s however, there is some difference.  The trouble
// seems to be that the expression `-(-i - c)` can't be reduced to merely `i +
// c`.  This optimization is turned on only if `-ffast-math` is explicitly
// enabled (`-O3` does not enable this by default).
void test_negating_lens1(double& i, double c) {
    auto neg = make_negating_lens(i);
    neg -= c;
}
void test_negating_lens2(double& i, double c) {
    i += c;
}

int main() {
    double r = 25;
    test_negating_lens1(r, 5);
    assert(r == 30);
    test_negating_lens2(r, 5);
    assert(r == 35);
    make_negating_lens(r) += 5;
    assert(r == 30);
    make_negating_lens(make_negating_lens(r)) += 5;
    assert(r == 35);
    make_negating_lens(make_negating_lens(r)) -= 5;
    assert(r == 30);
    return 0;
}
