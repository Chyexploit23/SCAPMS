#pragma once

#include <exception>
#include <string>

namespace Utils {

class ClinicException : public std::exception {
private:
    std::string message;

public:
    explicit ClinicException(const std::string& msg) : message(msg) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
};

} // namespace Utils