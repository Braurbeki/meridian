#include "resolve/ResolverRegistry.h"
#include "resolve/resolvers/DisplayText.h"

#include "core/MediaSource.h"

namespace mer::resolve {
namespace {

class MediaStatusResolver : public Resolver {
public:
    std::string resolve(const ResolveContext& ctx) const override
    {
        if (!ctx.media) {
            return "No media";
        }
        if (!ctx.media->online()) {
            return "Offline";
        }
        return ctx.media->streams().empty() ? "Online (not probed)" : "Online";
    }

    std::string description() const override { return "Media availability"; }
};

} // namespace
} // namespace mer::resolve

MER_REGISTER_RESOLVER(media, status, ::mer::resolve::MediaStatusResolver)
