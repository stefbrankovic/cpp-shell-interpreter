#ifndef COMMAND_H
#define COMMAND_H

#include <fstream>
#include <string>
#include <vector>

// Represents one token parsed from the command line.
struct Param {
	std::string content;
	bool isQuoted;
};

// Abstract base class for all shell commands.
class Command {
public:
	Command() = default;
	virtual ~Command() = default;

	// Set method called by Shell before execute() to supply parsed parameters.
	void setParameters(const std::vector<Param>& parameters);

	// Abstract method for stream reading and writting. 
	virtual void execute(std::istream& in, std::ostream& out, std::ostream& error) = 0;

protected:
	// Parameters supplied by Shell::setupInputStream after stripping the input-source token. 
	// Contains only options and semantic arguments.
	std::vector<Param> parameters_;

	// Checks does the readable file with given name exists.
	bool fileExists(const std::string& name);

	// Checks the token content for command option.
	const bool isOption(const Param& paramters);

	
};

#endif
