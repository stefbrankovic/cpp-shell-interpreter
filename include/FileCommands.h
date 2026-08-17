#ifndef FILECOMMANDS_H
#define FILECOMMANDS_H

#include "Command.h"
#include <string>

// Creates an empty file with the given name.
class TouchCommand : public Command {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;
};

// Deletes the content of an existing file.
class TruncateCommand : public Command {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;
};

// Removes a file from the directory.
class RmCommand : public Command {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;
};

#endif


