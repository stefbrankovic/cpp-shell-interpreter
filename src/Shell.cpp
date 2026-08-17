#include "BatchCommand.h"
#include "Errors.h"
#include "FileCommands.h"
#include "Shell.h"
#include "SystemCommands.h"
#include "TextCommands.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

Shell::Shell() : prompt_("$"), isRunning_(true) {
    registerCommands();
}

Shell::~Shell() {
    for (auto& pair : commands_) 
        delete pair.second;
    commands_.clear();
}

void Shell::run() {
    string line;
    while (isRunning_) {
        cout << prompt_ << " " << flush;

        if (!getline(cin, line)) {
            if (cin.eof()) {
                cin.clear();
                cout << endl;
                continue;
            }
            break;
        }

        // Strip trailing \r  and \n.
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
            line.pop_back();

        if (line.empty()) 
            continue;

        try {
            vector<CommandRun> pipeline = Parser::parse(line);
            if (!pipeline.empty())
                executePipeline(pipeline, cin, cout);
        }
        catch (const exception& e) {
            cerr << e.what() << endl;
        }
       
        cout << endl;
    }
}

void Shell::setPrompt(const std::string& newPrompt) {
    prompt_ = newPrompt;
}

void Shell::terminate() {
    isRunning_ = false;
}

void Shell::registerCommands() {
    commands_["echo"] = new EchoCommand();
    commands_["time"] = new TimeCommand();
    commands_["date"] = new DateCommand();
    commands_["prompt"] = new PromptCommand(this);
    commands_["touch"] = new TouchCommand();
    commands_["truncate"] = new TruncateCommand();
    commands_["rm"] = new RmCommand();
    commands_["wc"] = new WcCommand();
    commands_["tr"] = new TrCommand();
    commands_["head"] = new HeadCommand();
    commands_["batch"] = new BatchCommand(this);
}

// Private helper function that returns the index of the parameter that defines the input source, or -1 if none exists.
// Special case for 'tr' command (tr [input] -"what" ["with"]).
static int findInputParam(const std::string& cmdName, const std::vector<Param>& params){
    if (cmdName == "tr") {
        for (int i = 0; i < (int)params.size(); i++) {
            const Param& p = params[i];
           
            if (!p.isQuoted && p.content == "-" && i + 1 < (int)params.size() && params[i + 1].isQuoted){
                i++;
                continue;
            }
            
            if (!p.isQuoted && !p.content.empty() && p.content[0] == '-')
                continue;
            
            return i;
        }
        return -1;
    }

    for (int i = 0; i < (int)params.size(); i++) {
        const Param& p = params[i];
        if (!p.isQuoted && !p.content.empty() && p.content[0] == '-') 
            continue;
        return i;
    }
    return -1;
}


std::istream* Shell::setupInputStream(const CommandRun& run,
    int stageIndex,
    const std::string& pipeData,
    std::istream& defaultIn,
    std::ifstream& fileStream,
    std::istringstream& stringStream,
    std::istringstream& pipeStream,
    std::vector<Param>& strippedParams)
{
    // Start with a copy of all parameters.
    strippedParams = run.parameters;

   // Case 1: mid-pipeline — wrap previous stage's output string in a stream.
    if (stageIndex > 0) {
        pipeStream.str(pipeData);
        pipeStream.clear();
        return &pipeStream;
    }
  
    // Case 2: explicit '<' redirect — open the named file.
    if (!run.inputSource.empty()) {
        fileStream.open(run.inputSource);
        if (!fileStream.is_open())
            throw RuntimeError("Cannot open input file: " + run.inputSource);
        return &fileStream;
    }

    // Commands that accept a parameter-based input source.
    static const vector<string> streamCommands = { "echo", "wc", "tr", "head" };

    bool isStreamCmd = false;
    for (const auto& name : streamCommands) {
        if (run.commandName == name) { 
            isStreamCmd = true; 
            break; 
        }
    }

    if (isStreamCmd) {
        int index = findInputParam(run.commandName, strippedParams);

        if (index >= 0) {
            const Param& p = strippedParams[index];
            if (p.isQuoted) {
                // Case 3: quoted argument — the text itself is the stream content.
                stringStream.str(p.content);
                stringStream.clear();
                strippedParams.erase(strippedParams.begin() + index);
                return &stringStream;
            }
            else {
                // Case 4: unquoted argument — open as filename.
                fileStream.open(p.content);
                if (!fileStream.is_open())
                    throw RuntimeError("Cannot open input file: " + p.content);
                strippedParams.erase(strippedParams.begin() + index);
                return &fileStream;
            }
        }

        // Case 5: stream command but no argument found — buffer keyboard until EOF.
        std::string buffer, line;
        while (std::getline(defaultIn, line))
            buffer += line + '\n';
        defaultIn.clear();
        stringStream.str(buffer);
        stringStream.clear();
        return &stringStream;
    }

    // Case 6: non-stream command — pass defaultIn unchanged.
    return &defaultIn;
}

std::ostream* Shell::setupOutputStream(const CommandRun& run,
    int stageIndex,
    int pipelineSize,
    std::ostream& defaultOut,
    std::ofstream& fileStream,
    std::ostringstream& nextStage)
{
    if (!run.outputTarget.empty()) {
        auto mode = ios::out | (run.isAppend ? ios::app : ios::trunc);
        fileStream.open(run.outputTarget, mode);
        if (!fileStream.is_open())
            throw RuntimeError("Cannot open output file: " + run.outputTarget);
        return &fileStream;
    }

    // Mid-pipeline: write to an in-memory buffer; next stage will read it.
    if (stageIndex < pipelineSize - 1)
        return &nextStage;

    // Last (or only) stage: write to the caller's default output.
    return &defaultOut;
}


void Shell::executePipeline(const std::vector<CommandRun>& pipeline, std::istream& in, std::ostream& out) {
    string pipeData;

    for (int i = 0; i < (int)pipeline.size(); i++) {
        const CommandRun& run = pipeline[i];

        auto it = commands_.find(run.commandName);
        if (it == commands_.end())
            throw RuntimeError("Unknown command: " + run.commandName);

        Command* cmd = it->second;

        // Declare all stream objects on the stack for this stage.
        // They will be destroyed at the end of each iteration.
        ifstream inFile;
        istringstream inString;
        istringstream inPipe;
        ofstream outFile;
        ostringstream outPipe;
        vector<Param> strippedParams;

        istream* source = setupInputStream(run, i, pipeData, in, inFile, inString, inPipe, strippedParams);
        ostream* target = setupOutputStream(run, i, (int)pipeline.size(), out, outFile, outPipe);

        cmd->setParameters(strippedParams);
        cmd->execute(*source, *target, cerr);

        if (i < (int)pipeline.size() - 1)
            pipeData = outPipe.str();

        if (inFile.is_open())  
            inFile.close();

        if (outFile.is_open()) 
            outFile.close();
    }
}