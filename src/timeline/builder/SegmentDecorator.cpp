#include "timeline/builder/SegmentDecorator.h"

#include "core/TimeTypes.h"
#include "util/Log.h"
#include "util/StringUtil.h"

namespace mer::timeline {
namespace {

std::string truncateMiddle(const std::string& text, std::size_t limit)
{
    if (text.size() <= limit || limit < 5) {
        return text;
    }
    const std::size_t head = (limit - 1) / 2;
    const std::size_t tail = limit - 1 - head;
    return text.substr(0, head) + "…" + text.substr(text.size() - tail);
}

} // namespace

std::string SegmentDecorator::captionFor(const Segment& segment) const
{
    if (!segment.media()) {
        return {};
    }

    std::string caption = util::trim(segment.media()->fileName());
    if (settings_.truncateLabels && settings_.maxLabelChars > 0) {
        caption = truncateMiddle(caption,
                                 static_cast<std::size_t>(settings_.maxLabelChars));
    }
    return caption;
}

std::string SegmentDecorator::tooltipFor(const Segment& segment) const
{
    if (!segment.media()) {
        return "Offline media";
    }

    const core::MediaSourcePtr& media = segment.media();

    std::string out;
    out += "File: " + media->fileName() + "\n";
    out += "Path: " + media->path() + "\n";
    if (media->duration() > 0.0) {
        out += "Duration: " + core::formatDuration(media->duration()) + "\n";
    }
    if (const auto* video = media->primaryVideoStream()) {
        out += "Video: " + video->codec + " " + std::to_string(video->width) + "x"
             + std::to_string(video->height) + "\n";
    }
    if (!media->online()) {
        out += "Status: OFFLINE\n";
    }
    return out;
}

void SegmentDecorator::decorate(Segment& segment, const std::string& trackName,
                                int trackIndex) const
{
    (void)trackName;
    (void)trackIndex;

    segment.setLabel(captionFor(segment));
    segment.setTooltip(tooltipFor(segment));
    segment.setMediaOnline(segment.media() && segment.media()->online());

    MER_TRACE("timeline") << "decorated segment " << segment.id().toString()
                          << " label='" << segment.label() << "'";
}

} // namespace mer::timeline
