#ifndef ERRORS_H
#define ERRORS_H

#include <exception>
#include <string>

// Base class for all errors thrown by shell.
class ShellError : public std::exception {
public:
	ShellError(const std::string& message);

	virtual const char* what() const noexcept override;

protected:
	std::string message_;
};

// Thrown by Parser when the command line contains illegal characters or invalid syntax.
class LexicalError : public ShellError {
public:
	LexicalError(const std::string& error_details);
};

// Thrown during command execution.
class RuntimeError : public ShellError {
public:
	RuntimeError(const std::string& error_details);
};

#endif
