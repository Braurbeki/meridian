#pragma once
#include <string>
#include <vector>

namespace mer::app {

struct Options {
    std::string projectPath;
    bool        showHelp    = false;
    bool        showVersion = false;
};

Options parseCommandLine(const std::vector<std::string>& args);
std::string usageText();

} // namespace mer::app
