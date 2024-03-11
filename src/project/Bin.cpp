#include "project/Bin.h"

#include <algorithm>

namespace mer::project {

void Bin::add(ClipPtr clip)
{
    if (clip) {
        clips_.push_back(std::move(clip));
    }
}

bool Bin::remove(const util::Uuid& clipId)
{
    const auto it = std::find_if(clips_.begin(), clips_.end(),
                                 [&](const ClipPtr& c) { return c && c->id() == clipId; });
    if (it == clips_.end()) {
        return false;
    }
    clips_.erase(it);
    return true;
}

ClipPtr Bin::find(const util::Uuid& clipId) const
{
    for (const auto& c : clips_) {
        if (c && c->id() == clipId) {
            return c;
        }
    }
    for (const auto& child : children_) {
        if (ClipPtr found = child.find(clipId)) {
            return found;
        }
    }
    return nullptr;
}

} // namespace mer::project
