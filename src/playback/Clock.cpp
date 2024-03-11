#include "playback/Clock.h"

#include <cmath>

namespace mer::playback {

void Clock::start()
{
    if (running_) {
        return;
    }
    running_  = true;
    lastTick_ = SteadyClock::now();
}

void Clock::stop()
{
    running_ = false;
}

void Clock::reset(std::int64_t frame)
{
    frame_    = frame;
    residual_ = 0.0;
    lastTick_ = SteadyClock::now();
}

void Clock::seek(std::int64_t frame)
{
    frame_    = frame < 0 ? 0 : frame;
    residual_ = 0.0;
}

std::int64_t Clock::tick()
{
    if (!running_) {
        return frame_;
    }

    const auto   now     = SteadyClock::now();
    const double elapsed = std::chrono::duration<double>(now - lastTick_).count();
    lastTick_            = now;

    const double advanced = elapsed * rate_.toDouble() * speed_ + residual_;
    const double whole    = std::trunc(advanced);
    residual_             = advanced - whole;

    frame_ += static_cast<std::int64_t>(whole);
    if (frame_ < 0) {
        frame_    = 0;
        residual_ = 0.0;
    }
    return frame_;
}

} // namespace mer::playback
