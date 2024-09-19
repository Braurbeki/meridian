#pragma once
#include <string>
#include <vector>

namespace mer::app {

struct Options {
    std::string projectPath;
    bool        headless    = false;  ///< --dump-labels
    bool        showHelp    = false;
    bool        showVersion = false;
};

Options parseCommandLine(const std::vector<std::string>& args);
std::string usageText();

/// Headless entry point: builds the first sequence in the project and prints
/// one line per segment. Used by conform QC scripts and by the test suite.
int runDumpLabels(const std::string& projectPath);

} // namespace mer::app
