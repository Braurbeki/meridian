#pragma once
#include "core/MediaSource.h"
#include "core/MetadataDict.h"
#include "util/Uuid.h"

#include <string>

namespace mer::resolve {

/// Everything a resolver is allowed to see. Deliberately narrow: resolvers
/// must not reach back into the project graph or the UI, so that the same
/// resolver can run during headless export.
struct ResolveContext {
    util::Uuid               clipId;
    const core::MediaSource* media = nullptr;

    /// The name a cutter typed in the bin. Empty when never set.
    std::string editorialName;

    /// Editorial metadata attached to the clip instance (as opposed to the
    /// container tags on `media`). Populated by the ingest record.
    const core::MetadataDict* clipMetadata = nullptr;

    std::string trackName;
    int         trackIndex = -1;

    /// Locale-ish knobs the UI passes down.
    bool truncateForDisplay = true;
    int  maxDisplayChars    = 64;

    core::MetadataValue lookup(const std::string& key) const;
};

} // namespace mer::resolve
