#include "plugins/ExportPlugin.h"

#include "util/Log.h"

#include <fstream>

namespace mer::plugins {
namespace {

/// Writes a plain-text EDL-ish cut list. Uses the labels the timeline builder
/// already resolved, so the export matches what the cutter sees on screen.
class CutListExporter : public ExportPlugin {
public:
    std::string identifier() const override { return "com.meridian.export.cutlist"; }
    std::string displayName() const override { return "Cut List (text)"; }
    std::string fileExtension() const override { return "txt"; }

    bool exportTimeline(const timeline::Timeline& timeline,
                        const std::string& outputPath) override
    {
        std::ofstream out(outputPath);
        if (!out) {
            MER_ERROR("plugins") << "cannot write " << outputPath;
            return false;
        }

        out << "TITLE: " << timeline.name() << "\n";
        int index = 1;
        for (const auto& track : timeline.tracks()) {
            for (const auto& seg : track.segments()) {
                out << index++ << "\t" << track.header() << "\t" << seg.start()
                    << "\t" << seg.end() << "\t" << seg.label() << "\n";
            }
        }
        return true;
    }
};

} // namespace
} // namespace mer::plugins

MER_REGISTER_PLUGIN("com.meridian.export.cutlist", ::mer::plugins::CutListExporter)
