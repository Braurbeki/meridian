#include "playback/PlaybackEngine.h"

#include "util/Log.h"

#include <algorithm>

namespace mer::playback {

void PlaybackEngine::setTimeline(timeline::TimelinePtr timeline)
{
    timeline_ = std::move(timeline);
    cache_.clear();
    clock_.reset(0);
    if (timeline_) {
        clock_.setRate(timeline_->rate());
    }
    setState(TransportState::Stopped);
}

void PlaybackEngine::setState(TransportState s)
{
    if (state_ == s) {
        return;
    }
    state_ = s;
    if (stateCallback_) {
        stateCallback_(s);
    }
}

void PlaybackEngine::play()
{
    if (!timeline_) {
        MER_WARN("playback") << "play() with no timeline";
        return;
    }
    clock_.start();
    setState(TransportState::Playing);
}

void PlaybackEngine::pause()
{
    clock_.stop();
    setState(TransportState::Paused);
}

void PlaybackEngine::stop()
{
    clock_.stop();
    clock_.reset(0);
    setState(TransportState::Stopped);
    if (frameCallback_) {
        frameCallback_(0);
    }
}

void PlaybackEngine::seek(std::int64_t frame)
{
    const std::int64_t last = timeline_ ? timeline_->duration() : 0;
    clock_.seek(std::clamp<std::int64_t>(frame, 0, std::max<std::int64_t>(last - 1, 0)));
    if (frameCallback_) {
        frameCallback_(clock_.currentFrame());
    }
}

void PlaybackEngine::step(int frames)
{
    seek(clock_.currentFrame() + frames);
}

std::int64_t PlaybackEngine::update()
{
    if (state_ != TransportState::Playing) {
        return clock_.currentFrame();
    }

    const std::int64_t before = clock_.currentFrame();
    const std::int64_t now    = clock_.tick();

    if (timeline_ && now >= timeline_->duration()) {
        clock_.seek(std::max<std::int64_t>(timeline_->duration() - 1, 0));
        pause();
    }
    if (now != before && frameCallback_) {
        frameCallback_(clock_.currentFrame());
    }
    return clock_.currentFrame();
}

const timeline::Segment* PlaybackEngine::segmentUnderPlayhead(int trackIndex) const
{
    if (!timeline_ || trackIndex < 0
        || static_cast<std::size_t>(trackIndex) >= timeline_->tracks().size()) {
        return nullptr;
    }
    return timeline_->tracks()[trackIndex].segmentAt(clock_.currentFrame());
}

} // namespace mer::playback
