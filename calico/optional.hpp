#ifndef FEFMEVQFHPXPNWYXJLVS
#define FEFMEVQFHPXPNWYXJLVS
/// @file
///
/// This header implements a replacement for `std::optional`.
///
#include <stdexcept>
namespace cal {

/// A type that represents a missing value for `optional`.
class nullopt_t {};

/// A constant that represents a missing value for `optional`.
const nullopt_t nullopt = nullopt_t();

/// Represents an optional value.
template<class T>
class optional {
public:

    /// Creates a disengaged optional value.
    optional() : _engaged(false) {}

    /// Creates a disengaged optional value.
    optional(nullopt_t) : _engaged(false) {}

    /// Creates an engaged optional value.
    optional(const T& value) : _engaged(true), _value(value) {}

    optional(const optional& other) : _engaged(other._engaged) {
        if (_engaged)
            _value = other._value;
    }

    /// Returns the value of the `optional` if it exists, or the given `value`
    /// argument otherwise.
    template<class U>
    T value_or(const U& value) const {
        return _engaged ? _value : static_cast<T>(value);
    }

    /// Returns whether the `optional` is engaged.
    operator bool() const {
        return _engaged;
    }

    /// Disengages the value from the `optional`.
    optional& operator=(nullopt_t) { _engaged = false; }

    optional& operator=(const optional& other) {
        _engaged = other._engaged;
        _value = other._value;
    }

    /// Assigns a value to the `optional`.
    template<class U>
    optional& operator=(const U& value) {
        _engaged = true;
        _value = static_cast<T>(value);
    }

    /// Extracts the value from the `optional`.  If there is none, an
    /// `std::logic_error` is thrown.
    const T& value() const {
        if (!_engaged)
            throw std::logic_error(
                "error: cannot return value of disengaged optional.");
        return _value;
    }

    /// Extracts the value from the `optional`.  If there is none, an
    /// `std::logic_error` is thrown.
    T& value() {
        if (!_engaged)
            throw std::logic_error(
                "error: cannot return value of disengaged optional.");
        return _value;
    }

private:
    T _value;
    bool _engaged;
};

}
#endif
