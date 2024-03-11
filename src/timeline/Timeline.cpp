#include "timeline/Timeline.h"

#include <algorithm>

namespace mer::timeline {

std::int64_t Timeline::duration() const
{
    std::int64_t end = 0;
    for (const auto& t : tracks_) {
        end = std::max(end, t.duration());
    }
    return end;
}

std::size_t Timeline::segmentCount() const
{
    std::size_t n = 0;
    for (const auto& t : tracks_) {
        n += t.segments().size();
    }
    return n;
}

const Segment* Timeline::findSegment(const util::Uuid& id) const
{
    for (const auto& t : tracks_) {
        for (const auto& s : t.segments()) {
            if (s.id() == id) {
                return &s;
            }
        }
    }
    return nullptr;
}

} // namespace mer::timeline
