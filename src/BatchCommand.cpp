#include "BatchCommand.h"
#include "Parser.h"
#include "Shell.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

BatchCommand::BatchCommand(Shell* shell) : shell_(shell) {}

void BatchCommand::execute(std::istream& , std::ostream& out, std::ostream& error) {
    if (parameters_.empty()) {
        error << "Error: batch requires a filename." << endl;
        return;
    }

    const string filename = parameters_[0].content;
    ifstream file(filename);

    if (!file.is_open()) {
        error << "Error: batch: cannot open file '" << filename << "'." << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') 
            line.pop_back();

        if (line.empty()) 
            continue;

        try {
            vector<CommandRun> pipeline = Parser::parse(line);
            if (!pipeline.empty()) {
                shell_->executePipeline(pipeline, cin, out); 
                out << endl;
            }
        }
        catch (const exception& e) {
            error << e.what() << endl;
        }
    }
}