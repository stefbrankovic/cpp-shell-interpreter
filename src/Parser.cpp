#include "Errors.h"
#include "Parser.h"
#include <sstream>

using namespace std;

bool Parser::isIllegal(char c) {
   
    if (c == '\n' || c == '\r') 
        return false;   
    if ((unsigned char)c < 32)   //  Anything below ASCII 32 is a control character.
        return true;    
    
    static const string forbidden = "&*+?;`!@#%^={}[]\\";
    return forbidden.find(c) != string::npos;
}


std::vector<Param> Parser::tokenise(const string& line) {
    vector<Param> tokens;
    string current;
    // Tracks whether we are currently inside quotes
    bool inQuotes = false;
    // Tracks does the current token started with a quote.
    bool isQuoted = false;

   // Collect positions of all illiegal characters.
    vector<size_t> badPositions;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];

        // Quote handling.
        if (c == '"') {
            if (inQuotes) {               
                tokens.push_back({ current, true });
                current.clear();
                isQuoted = false;
                inQuotes = false;
            }
            else {              
                if (!current.empty()) {
                    tokens.push_back({ current, false });
                    current.clear();
                }
                inQuotes = true;
                isQuoted = true;
            }
            continue;
        }

        // Inside quotes, every character is legal.
        if (inQuotes) {         
            current += c;
            continue;
        }
      
        // Outside quotes

        if (isIllegal(c)) {
            badPositions.push_back(i);
            continue; 
        }

        // Whitespace is a token separator.
        if (isspace((unsigned char)c)) {
            if (!current.empty()) {
                tokens.push_back({ current, false });
                current.clear();
                isQuoted = false;
            }
            continue;
        }

        // Special operator characters.
        if (c == '|' || c == '<' || c == '>') {           
            if (!current.empty()) {
                tokens.push_back({ current, false });
                current.clear();
                isQuoted = false;
            }            
            if (c == '>' && i + 1 < line.size() && line[i + 1] == '>') {
                tokens.push_back({ ">>", false });
                i++;
            }
            else {
                tokens.push_back({ string(1, c), false });
            }
            continue;
        }
        current += c;
    }
   
    if (inQuotes) {
        throw LexicalError("unterminated quote in command line");
    }
 
    if (!current.empty()) {
        tokens.push_back({ current, false });
    }
    
    if (!badPositions.empty()) {
        string marker(line.size(), ' ');
        for (size_t pos : badPositions) 
            marker[pos] = '^';

        ostringstream message;
        message << "unexpected characters at position(s):\n"
            << line << "\n"
            << marker;
        throw LexicalError(message.str());
    }

    return tokens;
}

std::vector<CommandRun> Parser::parse(const std::string& rawLine) {    
    string line = rawLine;
    if ((int)line.size() > MAX_LEN) 
        line = line.substr(0, MAX_LEN);

    vector<Param> tokens = tokenise(line);   
    vector<CommandRun> pipeline;
    CommandRun current;

    for (size_t i = 0; i < tokens.size(); i++) {
        const Param& tok = tokens[i];

        // Pipeline
        if (!tok.isQuoted && tok.content == "|") {
            if (current.commandName.empty())
                throw LexicalError("'|' without a preceding command");
            pipeline.push_back(current);
            current = CommandRun();

        }
        // Redirection - inputSource
        else if (!tok.isQuoted && tok.content == "<") {
            if (i + 1 >= tokens.size())
                throw LexicalError("'<' without a filename");
            i++;
            current.inputSource = tokens[i].content;

        }
        // Redirection - outputTarget
        else if (!tok.isQuoted && (tok.content == ">" || tok.content == ">>")) {
            current.isAppend = (tok.content == ">>");
            if (i + 1 >= tokens.size())
                throw LexicalError("'>' without a filename");
            i++;
            current.outputTarget = tokens[i].content;
        }
        else {
            if (current.commandName.empty()) {
                current.commandName = tok.content;
            }
            else {
                current.parameters.push_back(tok);
            }
        }
    }

    if (!current.commandName.empty()) 
        pipeline.push_back(current);

    return pipeline;
}