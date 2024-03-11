#pragma once
#include "timeline/builder/SegmentDecorator.h"
#include "project/Project.h"
#include "timeline/Timeline.h"
#include "util/Diagnostics.h"

namespace mer::timeline {

/// Turns a project::TimelineSpec into a drawable Timeline.
///
/// The builder is the single place where editorial data becomes display data.
/// Views read the result and never recompute it, which is what keeps the
/// timeline, the bin and the conform report agreeing with one another.
///
/// Rebuilds are cheap (no media I/O) and are expected on every edit.
class TimelineBuilder {
public:
    explicit TimelineBuilder(const project::Project& project);

    void setDecoratorSettings(SegmentDecorator::Settings s) { decoratorSettings_ = s; }

    /// Builds `spec`. Never returns null; a spec referring to missing clips
    /// yields a timeline with those segments present but offline, and a
    /// diagnostic per missing reference.
    TimelinePtr build(const project::TimelineSpec& spec,
                      util::DiagnosticSink& sink) const;

private:
    Track buildTrack(const project::TrackSpec& trackSpec, int index,
                     const SegmentDecorator& decorator,
                     util::DiagnosticSink& sink) const;

    Segment materializeSegment(const project::SegmentSpec& spec,
                               util::DiagnosticSink& sink) const;

    const project::Project&    project_;
    SegmentDecorator::Settings decoratorSettings_;
};

} // namespace mer::timeline
