#pragma once
#include "core/MediaSource.h"
#include "util/Uuid.h"

#include <string>

namespace mer::timeline {

/// A materialised segment: a rectangle on a track, with everything the view
/// needs to draw it already computed. Views must not recompute display state
/// -- rebuild the timeline instead, so that panels stay consistent.
class Segment {
public:
    Segment() = default;

    const util::Uuid& id() const { return id_; }
    void              setId(const util::Uuid& id) { id_ = id; }

    const util::Uuid& clipId() const { return clipId_; }
    void              setClipId(const util::Uuid& id) { clipId_ = id; }

    std::int64_t start() const { return start_; }
    void         setStart(std::int64_t f) { start_ = f; }

    std::int64_t frameCount() const { return frameCount_; }
    void         setFrameCount(std::int64_t f) { frameCount_ = f; }

    std::int64_t end() const { return start_ + frameCount_; }

    /// Text drawn on the segment. Computed once by TimelineBuilder via the
    /// "segment.label" resolver; see docs/design/label-resolution.md.
    const std::string& label() const { return label_; }
    void               setLabel(std::string l) { label_ = std::move(l); }

    const std::string& tooltip() const { return tooltip_; }
    void               setTooltip(std::string t) { tooltip_ = std::move(t); }

    /// Cached so the view can grey out offline segments without touching the
    /// media library on every paint.
    bool mediaOnline() const { return mediaOnline_; }
    void setMediaOnline(bool v) { mediaOnline_ = v; }

    const core::MediaSourcePtr& media() const { return media_; }
    void setMedia(core::MediaSourcePtr m) { media_ = std::move(m); }

private:
    util::Uuid           id_ = util::Uuid::generate();
    util::Uuid           clipId_;
    std::int64_t         start_      = 0;
    std::int64_t         frameCount_ = 0;
    std::string          label_;
    std::string          tooltip_;
    bool                 mediaOnline_ = false;
    core::MediaSourcePtr media_;
};

} // namespace mer::timeline
