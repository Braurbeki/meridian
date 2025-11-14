#include "CommandLine.h"

#include "timeline/builder/TimelineBuilder.h"
#include "project/ProjectReader.h"
#include "resolve/ResolverRegistry.h"
#include "util/Diagnostics.h"
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
        } else if (arg == "--dump-labels") {
            options.headless = true;
        } else if (arg == "--list-resolvers") {
            options.listResolvers = true;
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
        "  --dump-labels      Build the first sequence and print segment captions\n"
        "  --list-resolvers   Print every registered field resolver key\n"
        "  --version          Print version and exit\n"
        "  -h, --help         Show this help\n"
        "\n"
        "Environment:\n"
        "  MERIDIAN_LOG_CHANNELS   Comma-separated channels to raise to trace\n"
        "                          level, or '*' for all. Channels in use:\n"
        "                          project, media, timeline, resolve, playback,\n"
        "                          plugins, compat, diag\n";
}

int runListResolvers()
{
    for (const auto& key : resolve::ResolverRegistry::instance().keys()) {
        std::cout << key << "\n";
    }
    return 0;
}

int runDumpLabels(const std::string& projectPath)
{
    util::DiagnosticSink sink;

    const project::ProjectReader reader;
    const auto                   project = reader.read(projectPath, sink);
    if (!project) {
        for (const auto& d : sink.entries()) {
            std::cerr << d.code << ": " << d.message << "\n";
        }
        return 1;
    }

    project->media().refreshAll(sink);

    if (project->timelines().empty()) {
        std::cerr << "Project contains no sequences\n";
        return 1;
    }

    const timeline::TimelineBuilder builder(*project);
    const auto built = builder.build(project->timelines().front(), sink);

    std::cout << "sequence: " << built->name() << "\n";
    for (const auto& track : built->tracks()) {
        std::cout << "track: " << track.header() << "\n";
        for (const auto& segment : track.segments()) {
            std::cout << "  [" << segment.start() << ".." << segment.end() << "] "
                      << "label='" << segment.label() << "'\n";
        }
    }

    for (const auto& d : sink.entries()) {
        if (d.severity != util::Severity::Info) {
            std::cerr << d.code << ": " << d.message << "\n";
        }
    }
    return 0;
}

} // namespace mer::app
