#include "Errors.h"
#include "FileCommands.h"

#include <cstdio>
#include <fstream>

using namespace std;

void TouchCommand::execute(std::istream& /*in*/, std::ostream& /*out*/, std::ostream& error) {
    if (parameters_.empty()) {
        error << "Error: touch requires a filename." << std::endl;
        return;
    }

    for (const auto& p : parameters_) {
        if (fileExists(p.content)) {
            error << "Error: touch: file '" << p.content << "' already exists." << endl;
            continue;
        }

        ofstream file(p.content);
        if (!file.is_open()) 
            error << "Error: touch: cannot create file '" << p.content << "'." << endl;
    }
}


void TruncateCommand::execute(std::istream& /*in*/, std::ostream& /*out*/, std::ostream& error) {
    if (parameters_.empty()) {
        error << "Error: truncate requires a filename." << endl;
        return;
    }

    for (const auto& p : parameters_) {
        if (!fileExists(p.content)) {
            error << "Error: truncate: file '" << p.content << "' does not exist." << endl;
            continue;
        }

        ofstream file(p.content, ios::out | ios::trunc);
        if (!file.is_open()) 
            error << "Error: truncate: cannot open file '" << p.content << "'." << endl;
    }
}


void RmCommand::execute(std::istream& /*in*/, std::ostream& /*out*/, std::ostream& error) {
    if (parameters_.empty()) {
        error << "Error: rm requires a filename." << endl;
        return;
    }

    for (const auto& p : parameters_) {
        if (!fileExists(p.content)) {
            error << "Error: rm: file '" << p.content << "' does not exist." << endl;
            continue;
        }

        if (remove(p.content.c_str()) != 0) 
            error << "Error: rm: cannot delete file '" << p.content << "'." << endl;
    }
}