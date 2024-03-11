#pragma once
#include "core/TimeTypes.h"

#include <chrono>

namespace mer::playback {

/// Monotonic transport clock. Deliberately free of Qt so the engine can be
/// driven from a test harness with no event loop.
class Clock {
public:
    void start();
    void stop();
    void reset(std::int64_t frame = 0);

    bool running() const { return running_; }

    void setRate(core::Rational rate) { rate_ = rate; }
    core::Rational rate() const { return rate_; }

    /// Playback speed multiplier: 1.0 normal, negative for reverse.
    void   setSpeed(double s) { speed_ = s; }
    double speed() const { return speed_; }

    /// Advances to wall-clock now and returns the current frame.
    std::int64_t tick();
    std::int64_t currentFrame() const { return frame_; }
    void         seek(std::int64_t frame);

private:
    using SteadyClock = std::chrono::steady_clock;

    core::Rational          rate_{25, 1};
    double                  speed_   = 1.0;
    bool                    running_ = false;
    std::int64_t            frame_   = 0;
    double                  residual_ = 0.0;
    SteadyClock::time_point lastTick_;
};

} // namespace mer::playback
