#pragma once

#include <stdexcept>

class SyntaxError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class RuntimeError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class NameError : public std::runtime_error {
public:
    explicit NameError(const std::string& name) : std::runtime_error{"Name not found: " + name} {
    }
};
