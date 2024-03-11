#pragma once
#include "core/MetadataDict.h"
#include "core/TimeTypes.h"
#include "util/Uuid.h"

#include <memory>
#include <string>
#include <vector>

namespace mer::core {

enum class StreamKind { Unknown, Video, Audio, Subtitle, Data };

struct StreamInfo {
    int         index = -1;
    StreamKind  kind  = StreamKind::Unknown;
    std::string codec;
    Rational    timeBase{1, 1000};
    Rational    frameRate{0, 1};
    int         width    = 0;
    int         height   = 0;
    int         channels = 0;
    int         sampleRate = 0;
    MetadataDict metadata;
};

/// A file on disk that the project references. One MediaSource may back many
/// clips; it owns the container-level facts and nothing about editorial use.
class MediaSource {
public:
    MediaSource() = default;
    explicit MediaSource(std::string path);

    const util::Uuid&  id() const { return id_; }
    void               setId(const util::Uuid& id) { id_ = id; }

    /// Path as recorded in the project file. May be relative to the project
    /// root and may point at media that is currently offline.
    const std::string& path() const { return path_; }
    void               setPath(std::string p) { path_ = std::move(p); }

    /// Absolute path once resolved against the project root; empty until
    /// the media manager has located the file.
    const std::string& resolvedPath() const { return resolvedPath_; }
    void               setResolvedPath(std::string p) { resolvedPath_ = std::move(p); }

    bool online() const { return online_; }
    void setOnline(bool v) { online_ = v; }

    double duration() const { return duration_; }
    void   setDuration(double d) { duration_ = d; }

    const MetadataDict& metadata() const { return metadata_; }
    MetadataDict&       metadata() { return metadata_; }

    const std::vector<StreamInfo>& streams() const { return streams_; }
    void addStream(StreamInfo s) { streams_.push_back(std::move(s)); }

    const StreamInfo* primaryVideoStream() const;
    const StreamInfo* primaryAudioStream() const;

    /// The file name component of path(), e.g. "A001_C012_0714XY.mxf".
    std::string fileName() const;

private:
    util::Uuid              id_ = util::Uuid::generate();
    std::string             path_;
    std::string             resolvedPath_;
    bool                    online_   = false;
    double                  duration_ = 0.0;
    MetadataDict            metadata_;
    std::vector<StreamInfo> streams_;
};

using MediaSourcePtr = std::shared_ptr<MediaSource>;

} // namespace mer::core
