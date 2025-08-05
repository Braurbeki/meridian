#include "timeline/builder/SegmentDecorator.h"

#include "resolve/ResolverRegistry.h"
#include "util/Log.h"

namespace mer::timeline {

resolve::ResolveContext SegmentDecorator::makeContext(const Segment& segment,
                                                      const std::string& trackName,
                                                      int trackIndex) const
{
    resolve::ResolveContext ctx;
    ctx.clipId             = segment.clipId();
    ctx.media              = segment.media().get();
    ctx.trackName          = trackName;
    ctx.trackIndex         = trackIndex;
    ctx.truncateForDisplay = settings_.truncateLabels;
    ctx.maxDisplayChars    = settings_.maxLabelChars;

    if (const project::ClipPtr clip = project_.findClip(segment.clipId())) {
        ctx.editorialName = clip->name();
        ctx.clipMetadata  = &clip->metadata();
    }
    return ctx;
}

void SegmentDecorator::decorate(Segment& segment, const std::string& trackName,
                                int trackIndex) const
{
    const resolve::ResolveContext ctx = makeContext(segment, trackName, trackIndex);
    const auto&                   registry = resolve::ResolverRegistry::instance();

    segment.setLabel(registry.resolve("segment.label", ctx));
    segment.setTooltip(registry.resolve("segment.tooltip", ctx));
    segment.setMediaOnline(segment.media() && segment.media()->online());

    MER_TRACE("timeline") << "decorated segment " << segment.id().toString()
                          << " label='" << segment.label() << "'";
}

} // namespace mer::timeline
