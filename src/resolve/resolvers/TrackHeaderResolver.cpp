#include "resolve/ResolverRegistry.h"
#include "resolve/resolvers/DisplayText.h"

namespace mer::resolve {
namespace {

class TrackHeaderResolver : public Resolver {
public:
    std::string resolve(const ResolveContext& ctx) const override
    {
        if (!ctx.trackName.empty()) {
            return applyDisplayLimits(ctx.trackName, ctx);
        }
        return ctx.trackIndex >= 0 ? "V" + std::to_string(ctx.trackIndex + 1)
                                   : std::string("--");
    }

    std::string description() const override { return "Track header caption"; }
};

} // namespace
} // namespace mer::resolve

MER_REGISTER_RESOLVER(track, header, ::mer::resolve::TrackHeaderResolver)
