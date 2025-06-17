#include "resolve/ResolveContext.h"

namespace mer::resolve {

core::MetadataValue ResolveContext::lookup(const std::string& key) const
{
    // Clip-level metadata shadows container tags: a colourist retagging a
    // clip in the bin should win over whatever the camera wrote.
    if (clipMetadata) {
        const core::MetadataValue v = clipMetadata->get(key);
        if (v.isSet()) {
            return v;
        }
    }
    if (media) {
        return media->metadata().get(key);
    }
    return {};
}

} // namespace mer::resolve
