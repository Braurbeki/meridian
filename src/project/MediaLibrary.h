#pragma once
#include "core/MediaProbe.h"
#include "core/MediaSource.h"
#include "util/Diagnostics.h"
#include "util/Uuid.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace mer::project {

/// Owns every MediaSource a project references and resolves recorded paths
/// against the project root. Relinking rewrites resolvedPath() only; the
/// path stored in the project file is left alone until the user saves.
class MediaLibrary {
public:
    void setProjectRoot(std::string root) { projectRoot_ = std::move(root); }
    const std::string& projectRoot() const { return projectRoot_; }

    core::MediaSourcePtr add(core::MediaSourcePtr source);
    core::MediaSourcePtr find(const util::Uuid& id) const;
    core::MediaSourcePtr findByPath(const std::string& path) const;

    std::vector<core::MediaSourcePtr> all() const;
    std::size_t                       size() const { return byId_.size(); }

    /// Resolves and probes every source. Offline media is reported through
    /// `sink` but does not abort the pass.
    void refreshAll(util::DiagnosticSink& sink);

    std::string resolvePath(const std::string& recordedPath) const;

private:
    std::string                                            projectRoot_;
    std::unordered_map<util::Uuid, core::MediaSourcePtr>   byId_;
};

} // namespace mer::project
