#include "Errors.h"
#include "TextCommands.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

std::istream* TextCommand::getInputStream(std::istream& defaultIn, std::ifstream& fileStream, std::string& foundFilename) {
    foundFilename.clear();

    // Find the filename (first non-option, non-quoted parameter).
    for (const auto& p : parameters_) {
        if (!isOption(p) && !p.isQuoted) {
            foundFilename = p.content;
            break;
        }
    }
    if (foundFilename.empty()) return &defaultIn;
    if (!fileExists(foundFilename)) return nullptr;
    fileStream.open(foundFilename);
    return &fileStream;
}


void WcCommand::execute(std::istream& in, std::ostream& out, std::ostream& error) {
    bool countWords = false, countChars = false;

    for (const auto& p : parameters_) {
        if (isOption(p)) {
            if (p.content == "-w") countWords = true;
            else if (p.content == "-c") countChars = true;
            else {
                error << "Error: wc: unknown option '" << p.content << "'." << endl;
                return;
            }
        }
    }

    // No option supplied - count both -w and -c.
    if (!countWords && !countChars)  
        countWords = countChars = true; 

    int  words = 0, chars = 0;
    bool inWord = false;
    char ch;

    while (in.get(ch)) {
        chars++;
        if (std::isspace((unsigned char)ch)) 
            inWord = false;
        else if (!inWord) { 
            inWord = true; 
            words++; 
        }
    }

    if (countWords && countChars) 
        out << words << " " << chars << endl;
    else if (countWords)          
        out << words << endl;
    else                          
        out << chars << endl;
}


void TrCommand::execute(std::istream& in, std::ostream& out, std::ostream& error) {
    std::string what, with;

    for (int i = 0; i < (int)parameters_.size(); i++) {
        const Param& p = parameters_[i];

        // Detect the -"what" pair: lone unquoted "-" followed by a quoted token.
        if (!p.isQuoted && p.content == "-") {
            if (i + 1 < (int)parameters_.size() && parameters_[i + 1].isQuoted) {
                what = parameters_[i + 1].content;
                i++;
                continue;
            }
        }

        // The next quoted token after "what" is the optional "with" replacement.
        if (p.isQuoted && !what.empty() && with.empty()) {
            with = p.content;
        }
    }

    if (what.empty()) {
        error << "Error: tr: missing -\"what\" argument." << endl;
        return;
    }

    // Read the entire input into a string at once.
    // istreambuf_iterator reads raw bytes directly from the stream buffer.
    string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

    if (with.empty()) {
        // Removal: build a new string, skipping every occurrence of 'what'.
        string result;
        for (size_t i = 0; i < content.size(); ) {
            if (content.compare(i, what.size(), what) == 0) 
                i += what.size();
            else 
                result += content[i++];
        }
        content = result;
    }
    else {
        // Replacement: find each occurrence and replace it with 'with'.
        size_t pos = 0;
        while ((pos = content.find(what, pos)) != string::npos) {
            content.replace(pos, what.size(), with);
            pos += with.size();
        }
    }

    out << content;
}


int HeadCommand::parseLineCount() const {
    for (int i = 0; i < (int)parameters_.size(); i++) {
        const string& s = parameters_[i].content;
        if (s.size() >= 2 && s[0] == '-' && s[1] == 'n') {
            if (s.size() > 2) {
                try { 
                    return stoi(s.substr(2)); 
                }
                catch (...) {}
            }
            else if (i + 1 < (int)parameters_.size()) {
                try { 
                    return stoi(parameters_[i + 1].content); 
                }
                catch (...) {}
            }
        }
    }
    return 10;
}


void HeadCommand::headFromStream(std::istream& in, std::ostream& out, int n) {
    vector<string> lines;
    string line;
    while (getline(in, line)) 
        lines.push_back(line);

    int limit = min(n, (int)lines.size());
    for (int i = 0; i < limit; i++) 
        out << lines[i] << endl;
}

void HeadCommand::headFromFile(std::istream& in, std::ostream& out, int n) {
    string line;
    for (int i = 0; i < n && getline(in, line); i++)
        out << line << endl;
}

void HeadCommand::execute(std::istream& in, std::ostream& out, std::ostream&) {
    headFromStream(in, out, parseLineCount());
}