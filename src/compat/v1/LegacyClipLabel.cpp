#include "compat/v1/LegacyClipLabel.h"

#include "util/StringUtil.h"

namespace mer::compat::v1 {

std::string legacyClipLabel(const project::Clip& clip, const core::MediaSource* media)
{
    if (media && !media->fileName().empty()) {
        return media->fileName();
    }
    return clip.name().empty() ? std::string("(offline)") : clip.name();
}

std::string legacyBinCaption(const project::Clip& clip, const core::MediaSource* media)
{
    if (!clip.name().empty()) {
        return clip.name();
    }
    return media ? util::stemName(media->fileName()) : std::string("(offline)");
}

} // namespace mer::compat::v1
