#include "TestMain.h"

#include "playback/Clock.h"
#include "playback/FrameCache.h"

using namespace mer::playback;

namespace {

CachedFrame makeFrame(std::int64_t index, std::size_t bytes)
{
    CachedFrame f;
    f.frame  = index;
    f.width  = 16;
    f.height = 16;
    f.pixels.assign(bytes, 0);
    return f;
}

} // namespace

MER_TEST(storesAndRetrieves)
{
    FrameCache cache(1024);
    cache.put(makeFrame(1, 100));
    MER_CHECK(cache.get(1) != nullptr);
    MER_CHECK(cache.get(2) == nullptr);
}

MER_TEST(evictsLeastRecentlyUsed)
{
    FrameCache cache(250);
    cache.put(makeFrame(1, 100));
    cache.put(makeFrame(2, 100));
    cache.get(1);                    // frame 1 becomes most recent
    cache.put(makeFrame(3, 100));    // must evict frame 2

    MER_CHECK(cache.get(1) != nullptr);
    MER_CHECK(cache.get(2) == nullptr);
    MER_CHECK(cache.get(3) != nullptr);
}

MER_TEST(clockSeeksAndClamps)
{
    Clock clock;
    clock.seek(120);
    MER_CHECK_EQ(clock.currentFrame(), std::int64_t(120));
    clock.seek(-5);
    MER_CHECK_EQ(clock.currentFrame(), std::int64_t(0));
}
