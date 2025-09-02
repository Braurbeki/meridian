#include "resolve/ResolverRegistry.h"
#include "resolve/resolvers/DisplayText.h"

#include "core/MediaSource.h"

namespace mer::resolve {
namespace {

/// Name shown in the bin list and the Inspector header.
///
/// Same policy as segment.label (MER-3980), but this one is also used for
/// sorting and for the conform report, so a blank entry would be actively
/// harmful -- hence the content checks rather than bare presence checks.
class ClipDisplayNameResolver : public Resolver {
public:
    std::string resolve(const ResolveContext& ctx) const override
    {
        if (!ctx.editorialName.empty()) {
            return applyDisplayLimits(ctx.editorialName, ctx);
        }

        const core::MetadataValue title = ctx.lookup("title");
        if (title.hasContent()) {
            return applyDisplayLimits(title.asString(), ctx);
        }

        if (ctx.media) {
            return applyDisplayLimits(ctx.media->fileName(), ctx);
        }
        return "(unnamed)";
    }

    std::string description() const override
    {
        return "Name shown in bins and the Inspector";
    }
};

} // namespace
} // namespace mer::resolve

MER_REGISTER_RESOLVER(clip, displayname, ::mer::resolve::ClipDisplayNameResolver)
