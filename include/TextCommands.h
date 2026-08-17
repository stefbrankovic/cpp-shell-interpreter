#ifndef TEXTCOMMANDS_H
#define TEXTCOMMANDS_H

#include "Command.h"
#include <fstream>
#include <vector>
#include <string>

// Abstract base class for commands that process text streams (wc, tr, head).
class TextCommand : public Command {
protected:
    // Locates the input source from parameters_ and opens it.
    std::istream* getInputStream(std::istream& defaultIn, std::ifstream& fileStream, std::string& foundFilename);
};

// Counts words (-w) or characters (-c) in the input stream.
class WcCommand : public TextCommand {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;
};

// Finds and replaces a substring in the input stream.
class TrCommand : public TextCommand {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;
};

// Outputs the first n lines of the input stream.
class HeadCommand : public TextCommand {
public:
    void execute(std::istream& in, std::ostream& out, std::ostream& error) override;

private:
    // Parses the mandatory -n option; returns 10 if not found.
    int parseLineCount() const;

    // Collects all lines, then outputs the first n.
    void headFromStream(std::istream& in, std::ostream& out, int n);

    // Reads and outputs at most n lines, stopping early.
    void headFromFile(std::istream& in, std::ostream& out, int n);
};

#endif