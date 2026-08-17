#include "Shell.h"
#include "SystemCommands.h"
#include "Errors.h"

#include <ctime>
#include <iomanip>
#include <iostream>

using namespace std;

// Returns the current local time as a std::tm struct.
static tm getCurrentTime() {
    tm now = {};
    time_t t = time(nullptr);
    localtime_s(&now, &t);
    return now;
}


void EchoCommand::execute(std::istream& in, std::ostream& out, std::ostream& /*error*/) {
    char ch;
    while (in.get(ch)) 
        out.put(ch);
}


PromptCommand::PromptCommand(Shell* shell) : shell_(shell) {}

void PromptCommand::execute(std::istream& , std::ostream& , std::ostream& error) {
    if (parameters_.empty() || !parameters_[0].isQuoted) {
        error << "Error: prompt requires a quoted string argument." << endl;
        return;
    }
    shell_->setPrompt(parameters_[0].content);
}


void TimeCommand::execute(std::istream& , std::ostream& out, std::ostream& error) {
    if (!parameters_.empty()) {
        error << "Error: time takes no options or arguments." << endl;
        return;
    }
    tm now = getCurrentTime();
    out << setfill('0')
        << setw(2) << now.tm_hour << ':'
        << setw(2) << now.tm_min << ':'
        << setw(2) << now.tm_sec
        << endl;
}


void DateCommand::execute(std::istream& , std::ostream& out, std::ostream& error) {
    if (!parameters_.empty()) {
        error << "Error: date takes no options or arguments." << endl;
        return;
    }
    tm now = getCurrentTime();
    out << setfill('0')
        << setw(2) << now.tm_mday << '.'
        << setw(2) << (now.tm_mon + 1) << '.'
        << (now.tm_year + 1900) << '.'
        << endl;
}