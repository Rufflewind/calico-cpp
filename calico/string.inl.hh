#include <sstream>
#include <string>
namespace fz {

template<class T>
std::string to_string(const T& x) {
    std::ostringstream stream;
    stream << x;
    return stream.str();
}

// Overload trivial cases to reduce ambiguity.
inline std::string to_string(const std::string& s) { return s; }
inline std::string to_string(const char* s) { return s; }
inline std::string to_string(char c) { return std::string(1, c); }

namespace _priv {
template<class T, class String>
T parse(const String& s) {
    T x;
    std::istringstream stream(s);
    stream >> x;
    return x;
}
}

template<class T>
T parse(const std::string& s) { _priv::parse(s); }

template<class T>
T parse(const char* s) { _priv::parse(s); }

}
