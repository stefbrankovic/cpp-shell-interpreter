#ifndef PARSER_H
#define PARSER_H

#include "Command.h"
#include <string>
#include <vector>

// Holds one fully-parsed command from the command line, ready for execution.
struct CommandRun {
    std::string commandName;
    std::vector<Param> parameters;    // options and arguments
    std::string inputSource = "";     // filename after '<', empty if no redirect
    std::string outputTarget = "";    // filename after '>' or '>>', empty if no redirect
    bool isAppend = false;            // true if '>>' was used (instead of '>')
};

// Lexical analysis and parsing of command lines. All methods are static.
class Parser {
public:
    // Parses line and returns the pipeline it represents.
    static std::vector<CommandRun> parse(const std::string& line);

private:
    // Maximum command line length.
    static const int MAX_LEN = 512;

    // Splits line into a vector of Param tokens, honouring quoting rules and special symbols (|, <, >, >>).
    // Collects all illegal character positions.
    static std::vector<Param> tokenise(const std::string& line);

    // Checks for printable characters that are forbidden outside quotes. 
    static bool isIllegal(char c);
};

#endif