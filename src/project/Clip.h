#pragma once
#include "core/MetadataDict.h"
#include "core/TimeTypes.h"
#include "util/Uuid.h"

#include <memory>
#include <string>

namespace mer::project {

/// An editorial reference to a range of a MediaSource. Clips live in bins and
/// are instanced onto timelines as segments; the same clip may appear many
/// times on a timeline.
class Clip {
public:
    Clip() = default;
    Clip(util::Uuid mediaId, std::string name);

    const util::Uuid& id() const { return id_; }
    void              setId(const util::Uuid& id) { id_ = id; }

    const util::Uuid& mediaId() const { return mediaId_; }
    void              setMediaId(const util::Uuid& id) { mediaId_ = id; }

    /// Editorial name assigned by a cutter in the bin. May be empty, in which
    /// case display falls back through the label policy.
    const std::string& name() const { return name_; }
    void               setName(std::string n) { name_ = std::move(n); }

    /// Ingest record: whatever the ingest tool or the cutter attached to this
    /// clip instance. Shadows container tags during resolution.
    const core::MetadataDict& metadata() const { return metadata_; }
    core::MetadataDict&       metadata() { return metadata_; }

    std::int64_t startFrame() const { return startFrame_; }
    void         setStartFrame(std::int64_t f) { startFrame_ = f; }

    std::int64_t frameCount() const { return frameCount_; }
    void         setFrameCount(std::int64_t f) { frameCount_ = f; }

    core::Rational rate() const { return rate_; }
    void           setRate(core::Rational r) { rate_ = r; }

    double durationSeconds() const;

private:
    util::Uuid         id_ = util::Uuid::generate();
    util::Uuid         mediaId_;
    std::string        name_;
    core::MetadataDict metadata_;
    std::int64_t       startFrame_ = 0;
    std::int64_t       frameCount_ = 0;
    core::Rational     rate_{25, 1};
};

using ClipPtr = std::shared_ptr<Clip>;

} // namespace mer::project
