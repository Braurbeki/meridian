#include "playback/FrameCache.h"

#include "util/Log.h"

namespace mer::playback {

const CachedFrame* FrameCache::get(std::int64_t frame)
{
    const auto it = entries_.find(frame);
    if (it == entries_.end()) {
        return nullptr;
    }
    // Touching an entry must move it to the front, or the budget evicts the
    // frames we are actually scrubbing over.
    order_.erase(it->second.position);
    order_.push_front(frame);
    it->second.position = order_.begin();
    return &it->second.frame;
}

void FrameCache::put(CachedFrame frame)
{
    if (!frame.valid()) {
        return;
    }
    const std::int64_t key = frame.frame;

    const auto existing = entries_.find(key);
    if (existing != entries_.end()) {
        used_ -= existing->second.frame.byteSize();
        order_.erase(existing->second.position);
        entries_.erase(existing);
    }

    used_ += frame.byteSize();
    order_.push_front(key);
    entries_[key] = Entry{std::move(frame), order_.begin()};

    evictUntilFits();
}

void FrameCache::evictUntilFits()
{
    while (used_ > budget_ && !order_.empty()) {
        const std::int64_t victim = order_.back();
        const auto         it     = entries_.find(victim);
        if (it != entries_.end()) {
            used_ -= it->second.frame.byteSize();
            entries_.erase(it);
        }
        order_.pop_back();
    }
}

void FrameCache::clear()
{
    entries_.clear();
    order_.clear();
    used_ = 0;
}

} // namespace mer::playback
