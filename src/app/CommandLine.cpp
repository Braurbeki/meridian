#include "CommandLine.h"

#include "util/Log.h"

#include <iostream>

namespace mer::app {

Options parseCommandLine(const std::vector<std::string>& args)
{
    Options options;
    for (std::size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--version") {
            options.showVersion = true;
        } else if (!arg.empty() && arg[0] != '-') {
            options.projectPath = arg;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
        }
    }
    return options;
}

std::string usageText()
{
    return
        "Usage: meridian [options] [project.mrp]\n"
        "\n"
        "Options:\n"
        "  --version          Print version and exit\n"
        "  -h, --help         Show this help\n"
        "\n"
        "Environment:\n"
        "  MERIDIAN_LOG_CHANNELS   Comma-separated channels to raise to trace\n"
        "                          level, or '*' for all. Channels in use:\n"
        "                          project, media, timeline, playback,\n"
        "                          plugins, compat, diag\n";
}

} // namespace mer::app
