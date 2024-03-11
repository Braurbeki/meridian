#pragma once
#include "playback/Clock.h"
#include "playback/FrameCache.h"
#include "timeline/Timeline.h"

#include <functional>

namespace mer::playback {

enum class TransportState { Stopped, Playing, Paused, Scrubbing };

/// Drives the transport over a built timeline. Decoding is stubbed in this
/// build: the engine reports which segment is under the playhead and emits
/// placeholder frames, which is enough for timeline and conform work.
class PlaybackEngine {
public:
    using FrameCallback = std::function<void(std::int64_t frame)>;
    using StateCallback = std::function<void(TransportState state)>;

    void setTimeline(timeline::TimelinePtr timeline);
    const timeline::TimelinePtr& timeline() const { return timeline_; }

    void play();
    void pause();
    void stop();
    void seek(std::int64_t frame);
    void step(int frames);

    /// Call from the host's frame tick. Returns the current frame.
    std::int64_t update();

    TransportState state() const { return state_; }
    std::int64_t   currentFrame() const { return clock_.currentFrame(); }

    const timeline::Segment* segmentUnderPlayhead(int trackIndex = 0) const;

    void onFrame(FrameCallback cb) { frameCallback_ = std::move(cb); }
    void onStateChanged(StateCallback cb) { stateCallback_ = std::move(cb); }

    FrameCache&       cache() { return cache_; }
    const FrameCache& cache() const { return cache_; }

private:
    void setState(TransportState s);

    timeline::TimelinePtr timeline_;
    Clock                 clock_;
    FrameCache            cache_;
    TransportState        state_ = TransportState::Stopped;
    FrameCallback         frameCallback_;
    StateCallback         stateCallback_;
};

} // namespace mer::playback
