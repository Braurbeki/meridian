#pragma once
#include "playback/FrameCache.h"
#include "playback/VideoDecoder.h"
#include "util/Diagnostics.h"

#include <map>
#include <memory>
#include <string>

namespace mer::playback {

/// Serves decoded frames to anything that needs a picture: the program
/// monitor at the playhead, and the timeline's segment thumbnails.
///
/// Owns one decoder per media file and an LRU of decoded frames, so that
/// scrubbing back and forth over a cut does not re-decode. Media that cannot
/// be decoded (offline, audio-only, or a build without FFmpeg) simply yields
/// nothing and callers draw a slate.
class FrameSource {
public:
    explicit FrameSource(std::size_t cacheBytes = 96u * 1024u * 1024u)
        : cache_(cacheBytes)
    {
    }

    /// Decodes `path` at `seconds`, no wider than `maxWidth`. `cacheKey` must
    /// be unique per (path, time, width) the caller intends to reuse.
    bool frame(const std::string& path, double seconds, int maxWidth,
               std::int64_t cacheKey, CachedFrame& out);

    bool canDecode(const std::string& path);

    void clear();

    util::DiagnosticSink&       diagnostics() { return sink_; }
    const util::DiagnosticSink& diagnostics() const { return sink_; }

private:
    VideoDecoder* decoderFor(const std::string& path);

    std::map<std::string, std::unique_ptr<VideoDecoder>> decoders_;
    FrameCache                                           cache_;
    util::DiagnosticSink                                 sink_;
};

} // namespace mer::playback
