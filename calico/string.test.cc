#include <cassert>
#include <iostream>
#include "string.hh"

int main() {
    using namespace fz;
    auto orig = std::string("dflkaj;lfkajfs");
    auto v = str_to_vector(orig);
    assert(*(v.end() - 1) == '\0');
    assert(std::string(v.begin(), v.end() - 1) == orig);
    v[0] = 'X';
    assert(std::string(v.begin(), v.end() - 1) != orig);
    v[0] = 'd';
    assert(std::string(v.begin(), v.end() - 1) == orig);
    std::cout << format_str("%f", 3.2);
    return 0;
}
