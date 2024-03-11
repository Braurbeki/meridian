#pragma once
#include "project/Project.h"
#include "timeline/Segment.h"

namespace mer::timeline {

/// Fills in the display-only fields of a Segment (caption, tooltip, media
/// state).
///
/// Split out of TimelineBuilder so that the conform report can decorate
/// segments it builds itself without duplicating the rules. Nothing here
/// knows about Qt or painting.
class SegmentDecorator {
public:
    struct Settings {
        bool truncateLabels = true;
        int  maxLabelChars  = 64;
    };

    explicit SegmentDecorator(const project::Project& project)
        : project_(project)
    {
    }

    SegmentDecorator(const project::Project& project, Settings s)
        : project_(project), settings_(s)
    {
    }

    void decorate(Segment& segment, const std::string& trackName,
                  int trackIndex) const;

private:
    std::string captionFor(const Segment& segment) const;
    std::string tooltipFor(const Segment& segment) const;

    const project::Project& project_;
    Settings                settings_;
};

} // namespace mer::timeline
