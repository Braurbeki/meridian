#include "resolve/ResolverRegistry.h"
#include "resolve/resolvers/DisplayText.h"

#include "core/MediaSource.h"
#include "core/TimeTypes.h"

namespace mer::resolve {
namespace {

/// Multi-line tooltip shown when hovering a timeline segment. Always reports
/// the underlying file so that a cutter can identify media regardless of how
/// the segment is labelled.
class SegmentTooltipResolver : public Resolver {
public:
    std::string resolve(const ResolveContext& ctx) const override
    {
        if (!ctx.media) {
            return "Offline media";
        }

        std::string out;
        out += "File: " + ctx.media->fileName() + "\n";
        out += "Path: " + ctx.media->path() + "\n";

        if (ctx.media->duration() > 0.0) {
            out += "Duration: " + core::formatDuration(ctx.media->duration()) + "\n";
        }
        if (const auto* video = ctx.media->primaryVideoStream()) {
            out += "Video: " + video->codec + " "
                 + std::to_string(video->width) + "x"
                 + std::to_string(video->height) + "\n";
        }
        if (!ctx.media->online()) {
            out += "Status: OFFLINE\n";
        }
        return out;
    }

    std::string description() const override { return "Timeline segment tooltip"; }
};

} // namespace
} // namespace mer::resolve

MER_REGISTER_RESOLVER(segment, tooltip, ::mer::resolve::SegmentTooltipResolver)
