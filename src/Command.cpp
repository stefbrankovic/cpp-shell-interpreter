#include "Command.h"
#include <iostream>

using namespace std;

void Command::setParameters(const std::vector<Param>& parameters){
	parameters_ = parameters;
}

bool Command::fileExists(const std::string& name){
	if (name.empty()) 
		return false;
	ifstream f(name);
	return f.good();
	
}

const bool Command::isOption(const Param& paramters){
	// An option token: not quoted, non-empty, and starts with '-'
	return !paramters.isQuoted && !paramters.content.empty() && paramters.content[0] == '-';
}


