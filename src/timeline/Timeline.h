#pragma once
#include "core/TimeTypes.h"
#include "timeline/Track.h"

#include <memory>
#include <string>
#include <vector>

namespace mer::timeline {

/// A built sequence, ready to draw and play. Immutable from the view's
/// perspective: any edit goes back through TimelineBuilder.
class Timeline {
public:
    Timeline() = default;

    const util::Uuid& id() const { return id_; }
    void              setId(const util::Uuid& id) { id_ = id; }

    const std::string& name() const { return name_; }
    void               setName(std::string n) { name_ = std::move(n); }

    core::Rational rate() const { return rate_; }
    void           setRate(core::Rational r) { rate_ = r; }

    void add(Track t) { tracks_.push_back(std::move(t)); }

    const std::vector<Track>& tracks() const { return tracks_; }
    std::vector<Track>&       tracks() { return tracks_; }

    std::int64_t duration() const;
    std::size_t  segmentCount() const;

    const Segment* findSegment(const util::Uuid& id) const;

private:
    util::Uuid         id_ = util::Uuid::generate();
    std::string        name_;
    core::Rational     rate_{25, 1};
    std::vector<Track> tracks_;
};

using TimelinePtr = std::shared_ptr<Timeline>;

} // namespace mer::timeline
