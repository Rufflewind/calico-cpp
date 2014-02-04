#include <cassert>
#include <cstdio>
#include <iostream>
#include <list>
#include <vector>
#include <calico/iterator.hpp>
using namespace cal;

void statictest() {

    // random
    {
        std::vector<int>::const_iterator p;
        counted_iterator<std::vector<int>::const_iterator> r;
        // not sure why you want to do this ...
        counted_iterator<counted_iterator<std::vector<int>::const_iterator> > s;
        r + 5;
        5 + r;
        r += 2;
        r -= 2;
        r - r;
        r - 5;
        r - 2L;
        r - (char) 2;
        r - p;
        p - r;
        r > p;
        p > r;
        p < r;
        r < p;
        r >= p;
        p >= r;
        p <= p;
        p <= r;
        p <= s;
        r <= p;
        r <= r;
        r <= s;
        s <= p;
        s <= r;
        s <= s;
    }
    // bidir
    {
        // bidir
        std::list<int>::iterator p;
        counted_iterator<std::list<int>::iterator> r = p;
        *--r;
        r--;
        ++r;
        r++;
        // suppress warnings
        (void) (r == p);
        (void) (p == r);
        (void) (p != r);
        (void) (r != p);
    }
}


int main(int, char**) {
    integer_iterator<int> nats(0), end(37);
    auto adder = [](int x) { return x + 42; };
    int j = 42;
    for (auto&& i : transform(make_range(nats, end), adder)) {
        assert(i == j);
        ++j;
    }
    --j;
    for (auto&& i : reverse_range(transform(make_range(nats, end), adder))) {
        assert(i == j);
        --j;
    }
    ++j;
    auto&& rev = reverse_range(transform(make_range(nats, end), adder));
    for (auto&& i = rev.crbegin(), end = rev.crend(); i != end; ++i) {
        assert(*i == j);
        ++j;
    }

/* Expected output:
(0, 1, 2, 3)
(0, 1, 2, 4)
(0, 1, 3, 4)
(0, 2, 3, 4)
(1, 2, 3, 4)
(0, 1, 2, 5)
(0, 1, 3, 5)
(0, 2, 3, 5)
(1, 2, 3, 5)
(0, 1, 4, 5)
(0, 2, 4, 5)
(1, 2, 4, 5)
(0, 3, 4, 5)
(1, 3, 4, 5)
(2, 3, 4, 5)
(0, 1)
(0, 2)
(1, 2)
(0, 3)
(1, 3)
(2, 3)
(0)
(1)
(2)
*/
    for (auto&& i : iterate_sord<4>(6))
        ; //std::cout << i << std::endl;
    for (auto&& i : iterate_sord<2>(4))
        ; //std::cout << i << std::endl;
    for (auto&& i : iterate_sord<1>(3))
        ; //std::cout << i << std::endl;
    for (auto&& i : iterate_sord<0>(2))
        ; //std::cout << i << std::endl;

}
