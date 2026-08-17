#include "Shell.h"
#include <iostream>

using namespace std;

int main() {
    try {
        // Shell is created.
        Shell myShell;

        // Enters the interactive read-parse-execute loop.
        myShell.run();
    }
    catch (const exception& e) {
        // Catching triggers for fatal errors.
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        // Catch-all errors.
        cerr << "Unknown system error." << endl;
        return 1;
    }

    return 0;
}