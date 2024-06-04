#pragma once
#include "project/Project.h"
#include "util/Diagnostics.h"

#include <memory>
#include <string>

namespace mer::compat::v1 {

/// Imports pre-2024 documents (.mrpx, format versions 1-3) by translating
/// them into the current model. Compiled only when MERIDIAN_LEGACY_PROJECTS
/// is enabled.
///
/// Imported documents are marked so that legacyClipLabel() keeps being used
/// for their existing segments; anything the user adds afterwards resolves
/// through the modern policy.
class LegacyProjectImporter {
public:
    static bool canImport(const std::string& path);

    std::unique_ptr<project::Project> import(const std::string& path,
                                             util::DiagnosticSink& sink) const;

private:
    bool parseHeader(const std::string& path, int& versionOut) const;
};

} // namespace mer::compat::v1
