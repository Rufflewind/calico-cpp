#include <iostream>
#include <winentry/winentry.h>

int main(int argc, char **argv) {
    for (char **argi = argv; argi < argv + argc; ++argi)
        std::cout << *argi << std::endl;
    return 0;
}
