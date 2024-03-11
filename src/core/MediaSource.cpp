#include "core/MediaSource.h"

#include "util/StringUtil.h"

namespace mer::core {

MediaSource::MediaSource(std::string path) : path_(std::move(path)) {}

std::string MediaSource::fileName() const
{
    return util::baseName(path_);
}

const StreamInfo* MediaSource::primaryVideoStream() const
{
    for (const auto& s : streams_) {
        if (s.kind == StreamKind::Video) {
            return &s;
        }
    }
    return nullptr;
}

const StreamInfo* MediaSource::primaryAudioStream() const
{
    for (const auto& s : streams_) {
        if (s.kind == StreamKind::Audio) {
            return &s;
        }
    }
    return nullptr;
}

} // namespace mer::core
