#ifndef BATCHCOMMAND_H
#define BATCHCOMMAND_H

#include "Command.h"

class Shell; 

// Reads a text file and executes every line as a command line, continuing past individual errors.
// Uses a back-pointer to Shell to call executePipeline() for each line.
class BatchCommand : public Command {
public:
	BatchCommand(Shell* shell);

	void execute(std::istream& in, std::ostream& out, std::ostream& error) override;

private:
	Shell* shell_;
};

#endif

