#include "playback/FrameSource.h"

#include "util/Log.h"

namespace mer::playback {

VideoDecoder* FrameSource::decoderFor(const std::string& path)
{
    const auto it = decoders_.find(path);
    if (it != decoders_.end()) {
        return it->second->isOpen() ? it->second.get() : nullptr;
    }

    auto decoder = std::make_unique<VideoDecoder>(path);
    decoder->open(sink_);  // failure is cached too, so we only try once

    VideoDecoder* raw = decoder->isOpen() ? decoder.get() : nullptr;
    decoders_.emplace(path, std::move(decoder));
    return raw;
}

bool FrameSource::canDecode(const std::string& path)
{
    return decoderFor(path) != nullptr;
}

bool FrameSource::frame(const std::string& path, double seconds, int maxWidth,
                        std::int64_t cacheKey, CachedFrame& out)
{
    if (const CachedFrame* hit = cache_.get(cacheKey)) {
        out = *hit;
        return true;
    }

    VideoDecoder* decoder = decoderFor(path);
    if (!decoder) {
        return false;
    }

    CachedFrame decoded;
    if (!decoder->frameAt(seconds, maxWidth, decoded)) {
        return false;
    }

    decoded.frame = cacheKey;
    cache_.put(decoded);
    out = std::move(decoded);
    return true;
}

void FrameSource::clear()
{
    decoders_.clear();
    cache_.clear();
    sink_.clear();
}

} // namespace mer::playback
