#pragma once
#include <cstdint>
#include <list>
#include <unordered_map>
#include <vector>

namespace mer::playback {

/// Decoded frame, stored as packed RGB24. Small by design: this player only
/// caches proxy-resolution frames for scrubbing.
struct CachedFrame {
    std::int64_t             frame  = -1;
    int                      width  = 0;
    int                      height = 0;
    std::vector<std::uint8_t> pixels;

    bool valid() const { return frame >= 0 && width > 0 && height > 0; }
    std::size_t byteSize() const { return pixels.size(); }
};

/// LRU cache with a byte budget rather than a count budget, because frame
/// sizes vary by an order of magnitude between proxy and full resolution.
class FrameCache {
public:
    explicit FrameCache(std::size_t budgetBytes = 64u * 1024u * 1024u)
        : budget_(budgetBytes)
    {
    }

    const CachedFrame* get(std::int64_t frame);
    void               put(CachedFrame frame);
    void               clear();

    std::size_t bytesUsed() const { return used_; }
    std::size_t count() const { return entries_.size(); }
    std::size_t budget() const { return budget_; }

private:
    void evictUntilFits();

    using Order = std::list<std::int64_t>;

    struct Entry {
        CachedFrame          frame;
        Order::iterator      position;
    };

    std::size_t                                 budget_;
    std::size_t                                 used_ = 0;
    Order                                       order_;  ///< front = most recent
    std::unordered_map<std::int64_t, Entry>     entries_;
};

} // namespace mer::playback
