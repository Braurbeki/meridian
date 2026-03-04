#include "resolve/ResolverRegistry.h"
#include "resolve/resolvers/DisplayText.h"

#include "core/MediaSource.h"
#include "util/Log.h"
#include "util/StringUtil.h"

namespace mer::resolve {
namespace {

/// Label shown on a segment rectangle in the timeline.
///
/// Resolution order is fixed by MER-3980 (see docs/design/label-resolution.md):
///   1. the editorial name the cutter assigned in the bin
///   2. the title embedded by the ingest tool
///   3. the source file name
///
/// Before 2026.12 this was always step 3.
class SegmentLabelResolver : public Resolver {
public:
    std::string resolve(const ResolveContext& ctx) const override
    {
        if (!ctx.editorialName.empty()) {
            return applyDisplayLimits(ctx.editorialName, ctx);
        }

        const core::MetadataValue title = ctx.lookup("title");
        if (title.isSet()) {
            return applyDisplayLimits(title.asString(), ctx);
        }

        if (ctx.media) {
            return applyDisplayLimits(ctx.media->fileName(), ctx);
        }

        MER_DEBUG("resolve") << "segment.label: nothing to show for "
                             << ctx.clipId.toString();
        return {};
    }

    std::string description() const override
    {
        return "Name shown on timeline segments";
    }
};

std::string truncateMiddle(const std::string& text, std::size_t limit)
{
    if (text.size() <= limit || limit < 5) {
        return text;
    }
    const std::size_t head = (limit - 3) / 2;
    const std::size_t tail = limit - 3 - head;
    return text.substr(0, head) + "..." + text.substr(text.size() - tail);
}

} // namespace

std::string applyDisplayLimits(std::string text, const ResolveContext& ctx)
{
    text = util::trim(std::move(text));
    if (!ctx.truncateForDisplay || ctx.maxDisplayChars <= 0) {
        return text;
    }
    return truncateMiddle(text, static_cast<std::size_t>(ctx.maxDisplayChars));
}

} // namespace mer::resolve

MER_REGISTER_RESOLVER(segment, label, ::mer::resolve::SegmentLabelResolver)
