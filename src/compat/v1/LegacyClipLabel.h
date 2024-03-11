#pragma once
#include "project/Clip.h"
#include "core/MediaSource.h"

#include <string>

namespace mer::compat::v1 {

/// Pre-2024 label rule: the timeline always showed the source file name,
/// with the editorial name only in the bin.
///
/// Retained because imported v1 documents keep their original captions until
/// the user re-labels them; new documents go through the resolver registry
/// instead (see src/resolve). Do not call this for format >= 4 projects.
std::string legacyClipLabel(const project::Clip& clip,
                            const core::MediaSource* media);

/// The v1 caption rule for bins, which differed from the timeline one.
std::string legacyBinCaption(const project::Clip& clip,
                             const core::MediaSource* media);

} // namespace mer::compat::v1
