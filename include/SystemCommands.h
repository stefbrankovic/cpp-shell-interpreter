#ifndef SYSTEMCOMMANDS_H
#define SYSTEMCOMMANDS_H

#include "Command.h"

// Forward declaration (PromptCommand holds a Shell*)
class Shell;

// Copies its input stream to its output stream unchanged.
class EchoCommand : public Command {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;
};

// Changes the shell's command prompt string.
class PromptCommand : public Command {
public:
    explicit PromptCommand(Shell* shell);
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;

private:
    Shell* shell_;
};

// Outputs the current system time in HH:MM:SS format.
class TimeCommand : public Command {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;
};

// Outputs the current system date in DD.MM.YYYY. format.
class DateCommand : public Command {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;
};

#endif
