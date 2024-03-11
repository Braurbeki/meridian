#include "project/Clip.h"

namespace mer::project {

Clip::Clip(util::Uuid mediaId, std::string name)
    : mediaId_(mediaId), name_(std::move(name))
{
}

double Clip::durationSeconds() const
{
    const double fps = rate_.toDouble();
    return fps > 0.0 ? static_cast<double>(frameCount_) / fps : 0.0;
}

} // namespace mer::project
