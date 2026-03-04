#pragma once
#include "playback/FrameCache.h"
#include "util/Diagnostics.h"

#include <memory>
#include <string>

namespace mer::playback {

/// Pulls single frames out of a media file for display.
///
/// This is a review tool, not a transcoder: we decode on demand at the
/// playhead and at segment heads for thumbnails, and we never try to keep up
/// with realtime. Frames come back as packed RGB24 at or below a requested
/// width.
///
/// The FFmpeg types stay behind the PIMPL so that a build without FFmpeg
/// compiles this header unchanged; in that configuration open() reports
/// failure and the views fall back to a slate.
class VideoDecoder {
public:
    explicit VideoDecoder(std::string path);
    ~VideoDecoder();

    VideoDecoder(const VideoDecoder&)            = delete;
    VideoDecoder& operator=(const VideoDecoder&) = delete;

    bool open(util::DiagnosticSink& sink);
    bool isOpen() const;

    /// Decodes the frame covering `seconds`, downscaled so that it is no
    /// wider than `maxWidth`. Returns false if nothing could be decoded.
    bool frameAt(double seconds, int maxWidth, CachedFrame& out);

    int    nativeWidth() const;
    int    nativeHeight() const;
    double duration() const;

    static bool available();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace mer::playback
