#include <list>
#include <vector>
#include "iterator.hh"
using namespace fz;

int main() {
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
