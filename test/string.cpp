#include <cassert>
#include <iostream>
#include <calico/string.hpp>

int main() {
    using namespace cal;
    auto orig = std::string("dflkaj;lfkajfs");
    auto v = str_to_vector(orig);
    assert(*(v.end() - 1) == '\0');
    assert(std::string(v.begin(), v.end() - 1) == orig);
    v[0] = 'X';
    assert(std::string(v.begin(), v.end() - 1) != orig);
    v[0] = 'd';
    assert(std::string(v.begin(), v.end() - 1) == orig);
    std::cout << "3.200 <== Expect to see this\n"
              << format_str("%.3f", 3.2)
              << "\n";
    return 0;
}
