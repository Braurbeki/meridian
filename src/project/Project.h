#pragma once
#include "project/Bin.h"
#include "project/MediaLibrary.h"
#include "util/Diagnostics.h"

#include <string>
#include <vector>

namespace mer::project {

/// One segment as recorded in the project file: an instruction to place a
/// range of a clip at a position on a track. Carries no display state -- the
/// timeline builder derives all of that.
struct SegmentSpec {
    util::Uuid   id = util::Uuid::generate();
    util::Uuid   clipId;
    std::int64_t timelineStart = 0;  ///< frames from timeline origin
    std::int64_t sourceStart   = 0;  ///< frames into the clip
    std::int64_t frameCount    = 0;
};

struct TrackSpec {
    util::Uuid               id = util::Uuid::generate();
    std::string              name;
    std::string              kind = "video";  ///< "video" | "audio"
    bool                     muted = false;
    bool                     locked = false;
    std::vector<SegmentSpec> segments;
};

/// The serialised form of a sequence. Turned into a live timeline::Timeline
/// by TimelineBuilder.
struct TimelineSpec {
    util::Uuid             id = util::Uuid::generate();
    std::string            name;
    core::Rational         rate{25, 1};
    std::vector<TrackSpec> tracks;
};

/// Root of the document model. Owns media, bins and sequence specs.
class Project {
public:
    Project() = default;

    const std::string& name() const { return name_; }
    void               setName(std::string n) { name_ = std::move(n); }

    const std::string& filePath() const { return filePath_; }
    void               setFilePath(std::string p) { filePath_ = std::move(p); }

    MediaLibrary&       media() { return media_; }
    const MediaLibrary& media() const { return media_; }

    Bin&       rootBin() { return rootBin_; }
    const Bin& rootBin() const { return rootBin_; }

    const std::vector<TimelineSpec>& timelines() const { return timelines_; }
    std::vector<TimelineSpec>&       timelines() { return timelines_; }

    ClipPtr findClip(const util::Uuid& id) const { return rootBin_.find(id); }

    /// Convenience for panels: the media backing a clip, or nullptr.
    core::MediaSourcePtr mediaForClip(const util::Uuid& clipId) const;

    bool empty() const { return timelines_.empty() && rootBin_.size() == 0; }

private:
    std::string               name_ = "Untitled";
    std::string               filePath_;
    MediaLibrary              media_;
    Bin                       rootBin_{"Master"};
    std::vector<TimelineSpec> timelines_;
};

} // namespace mer::project
