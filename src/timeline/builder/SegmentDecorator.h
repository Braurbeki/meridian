#pragma once
#include "project/Project.h"
#include "resolve/ResolveContext.h"
#include "timeline/Segment.h"

namespace mer::timeline {

/// Fills in the display-only fields of a Segment (label, tooltip, media
/// state) by asking the resolver registry.
///
/// Split out of TimelineBuilder so that the bin list and the conform report
/// can decorate segments they build themselves without duplicating the
/// policy. Nothing here knows about Qt or painting.
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
    resolve::ResolveContext makeContext(const Segment& segment,
                                        const std::string& trackName,
                                        int trackIndex) const;

    const project::Project& project_;
    Settings                settings_;
};

} // namespace mer::timeline
