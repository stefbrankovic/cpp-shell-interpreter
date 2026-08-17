#ifndef SHELL_H
#define SHELL_H

#include "Command.h"
#include "Parser.h"

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// The "mind" of the shell interpreter.
class Shell {
public:
    Shell();
    ~Shell();

    // Starts the interactive loop (print prompt, read line, parse, execute).
    // Returns only when the user triggers EOF or an unrecoverable error occurs.
    void run();

    // Changes the prompt string shown before each command line.
    void setPrompt(const std::string& newPrompt);

    // Signals the shell to stop after the current command finishes.
    void terminate();

    // Executes a fully-parsed pipeline.
    void executePipeline(const std::vector<CommandRun>& pipeline, std::istream& in = std::cin, std::ostream& out = std::cout);

private:
    std::map<std::string, Command*> commands_;  // Command registry.
    std::string prompt_;    // current prompt string (initially "$").
    bool isRunning_;       // false causes run() to exit after current command.

    // Registers all supported commands into commands_ (single point of registration).
    void registerCommands();

    // Determines and returns the input stream for one pipeline stage.
    std::istream* setupInputStream(const CommandRun& run,
        int stageIndex,
        const std::string& pipeData,
        std::istream& defaultIn,
        std::ifstream& fileStream,
        std::istringstream& stringStream,
        std::istringstream& pipeStream,
        std::vector<Param>& strippedParams);

    // Determines and returns the output stream for one pipeline stage.
    std::ostream* setupOutputStream(const CommandRun& run,
        int stageIndex,
        int pipelineSize,
        std::ostream& defaultOut,
        std::ofstream& fileStream,
        std::ostringstream& nextStage);
};

#endif