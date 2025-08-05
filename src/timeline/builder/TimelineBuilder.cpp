#include "timeline/builder/TimelineBuilder.h"

#include "resolve/ResolverRegistry.h"
#include "util/Log.h"

namespace mer::timeline {
namespace {

TrackKind kindFromString(const std::string& s)
{
    return s == "audio" ? TrackKind::Audio : TrackKind::Video;
}

} // namespace

TimelineBuilder::TimelineBuilder(const project::Project& project, QObject* parent)
    : QObject(parent), project_(project)
{
}

Segment TimelineBuilder::materializeSegment(const project::SegmentSpec& spec,
                                            util::DiagnosticSink& sink) const
{
    Segment segment;
    segment.setId(spec.id);
    segment.setClipId(spec.clipId);
    segment.setStart(spec.timelineStart);
    segment.setFrameCount(spec.frameCount);

    const project::ClipPtr clip = project_.findClip(spec.clipId);
    if (!clip) {
        sink.warn("timeline.missingclip",
                  "Segment references a clip that is not in any bin",
                  spec.clipId.toString());
        return segment;
    }

    if (const core::MediaSourcePtr media = project_.media().find(clip->mediaId())) {
        segment.setMedia(media);
    } else {
        sink.warn("timeline.missingmedia",
                  "Clip references media that is not in the library",
                  clip->mediaId().toString());
    }

    // Display fields are filled in by the decorator, not here -- see
    // SegmentDecorator and docs/design/label-resolution.md.
    return segment;
}

Track TimelineBuilder::buildTrack(const project::TrackSpec& trackSpec, int index,
                                  const SegmentDecorator& decorator,
                                  util::DiagnosticSink& sink) const
{
    Track track;
    track.setId(trackSpec.id);
    track.setKind(kindFromString(trackSpec.kind));
    track.setMuted(trackSpec.muted);
    track.setLocked(trackSpec.locked);

    resolve::ResolveContext headerCtx;
    headerCtx.trackName  = trackSpec.name;
    headerCtx.trackIndex = index;
    track.setHeader(
        resolve::ResolverRegistry::instance().resolve("track.header", headerCtx));

    for (const auto& segSpec : trackSpec.segments) {
        Segment segment = materializeSegment(segSpec, sink);
        decorator.decorate(segment, trackSpec.name, index);
        track.add(std::move(segment));
    }

    return track;
}

TimelinePtr TimelineBuilder::build(const project::TimelineSpec& spec,
                                   util::DiagnosticSink& sink) const
{
    auto timeline = std::make_shared<Timeline>();
    timeline->setId(spec.id);
    timeline->setName(spec.name);
    timeline->setRate(spec.rate);

    const SegmentDecorator decorator(project_, decoratorSettings_);

    const_cast<TimelineBuilder*>(this)->buildStarted(
        static_cast<int>(spec.tracks.size()));

    for (std::size_t i = 0; i < spec.tracks.size(); ++i) {
        timeline->add(
            buildTrack(spec.tracks[i], static_cast<int>(i), decorator, sink));
        const_cast<TimelineBuilder*>(this)->trackBuilt(static_cast<int>(i));
    }

    const_cast<TimelineBuilder*>(this)->buildFinished(
        static_cast<qint64>(timeline->segmentCount()));

    MER_INFO("timeline") << "built '" << timeline->name() << "' tracks="
                         << timeline->tracks().size()
                         << " segments=" << timeline->segmentCount();
    return timeline;
}

} // namespace mer::timeline
