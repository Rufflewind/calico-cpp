#ifndef FZ_OPTIONAL_HH
#define FZ_OPTIONAL_HH
#include <stdexcept>
#include <cxx11.hh>
namespace fz {

struct nullopt_t {};

template<class T>
class optional {
    optional() : _engaged(false) {}
    optional(nullopt_t) : _engaged(false) {}
    optional(const T& value) : _engaged(true), _value(value) {}
    optional(const optional& other) : _engaged(other._engaged) {
        if (engaged)
            _value(other._value);
    }

    template<class U>
    T value_or(const U& value) const {
        return _engaged ? _value : static_cast<T>(value);
    }

    explicit operator bool() const {
        return _engaged;
    }

    optional& operator=(nullopt_t) { _engaged = false; }

    optional& operator=(const optional& other) {
        _engaged = other._engaged;
        _value = other._value;
    }

    template<class U>
    optional& operator=(const U& value) {
        _engaged = true;
        _value = static_cast<T>(value);
    }

    const T& value() const {
        if (!_engaged)
            throw std::logic_error(
                "error: cannot return value of disengaged optional.");
        return _value;
    }

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
