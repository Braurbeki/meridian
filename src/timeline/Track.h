#pragma once
#include "timeline/Segment.h"

#include <string>
#include <vector>

namespace mer::timeline {

enum class TrackKind { Video, Audio };

class Track {
public:
    Track() = default;

    const util::Uuid& id() const { return id_; }
    void              setId(const util::Uuid& id) { id_ = id; }

    /// Caption drawn in the track head, from the "track.header" resolver.
    const std::string& header() const { return header_; }
    void               setHeader(std::string h) { header_ = std::move(h); }

    TrackKind kind() const { return kind_; }
    void      setKind(TrackKind k) { kind_ = k; }

    bool muted() const { return muted_; }
    void setMuted(bool v) { muted_ = v; }
    bool locked() const { return locked_; }
    void setLocked(bool v) { locked_ = v; }

    void add(Segment s) { segments_.push_back(std::move(s)); }

    const std::vector<Segment>& segments() const { return segments_; }
    std::vector<Segment>&       segments() { return segments_; }

    const Segment* segmentAt(std::int64_t frame) const;
    std::int64_t   duration() const;

private:
    util::Uuid           id_ = util::Uuid::generate();
    std::string          header_;
    TrackKind            kind_   = TrackKind::Video;
    bool                 muted_  = false;
    bool                 locked_ = false;
    std::vector<Segment> segments_;
};

} // namespace mer::timeline
