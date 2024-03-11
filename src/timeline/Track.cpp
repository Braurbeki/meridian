#include "timeline/Track.h"

#include <algorithm>

namespace mer::timeline {

const Segment* Track::segmentAt(std::int64_t frame) const
{
    for (const auto& s : segments_) {
        if (frame >= s.start() && frame < s.end()) {
            return &s;
        }
    }
    return nullptr;
}

std::int64_t Track::duration() const
{
    std::int64_t end = 0;
    for (const auto& s : segments_) {
        end = std::max(end, s.end());
    }
    return end;
}

} // namespace mer::timeline
