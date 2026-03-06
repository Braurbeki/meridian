#pragma once
#include "core/MediaSource.h"
#include "util/Diagnostics.h"

#include <string>

namespace mer::core {

/// Reads container-level facts (duration, streams, tags) via libavformat.
///
/// Probing is best-effort: media referenced by a project is frequently
/// offline (unmounted volume, relinked storage), and a project must still
/// open and edit in that state.
class MediaProbe {
public:
    struct Options {
        bool readStreamMetadata = true;
        bool analyzeFrames      = false;  ///< slower; needed for VFR detection
        int  probeTimeoutMs     = 5000;
    };

    MediaProbe() = default;
    explicit MediaProbe(Options opts) : opts_(opts) {}

    /// Populates `source` in place. Returns false when the file could not be
    /// opened; `source` is left with online() == false and any diagnostics
    /// pushed to `sink`.
    bool probe(MediaSource& source, util::DiagnosticSink& sink) const;

    static std::string ffmpegVersionString();

private:
    Options opts_;
};

} // namespace mer::core
