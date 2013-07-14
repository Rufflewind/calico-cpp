#ifndef FZ_STRING_HPP
#define FZ_STRING_HPP
#include <string>
namespace fz {

/// Constructs a string representation of an object using the stream insertion
/// operator (`<<`).
template<class T>
std::string to_string(const T&);

/// Extracts an object of type `T` from a string using the stream extraction
/// operator (`>>`).
template<class T, class String>
T parse(const String&);

}
#include "string.inl.hpp"
#endif
