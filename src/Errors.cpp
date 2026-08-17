#include "Errors.h"

ShellError::ShellError(const std::string& message) : message_(message) {}

const char* ShellError::what() const noexcept {
    return message_.c_str();
}

LexicalError::LexicalError(const std::string& details)
    : ShellError("Lexical error: " + details) {}

RuntimeError::RuntimeError(const std::string& details)
    : ShellError("Runtime error: " + details) {}