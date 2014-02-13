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


int main() {
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
}
