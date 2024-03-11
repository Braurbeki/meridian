#pragma once
#include "project/Project.h"
#include "util/Diagnostics.h"

#include <memory>
#include <string>

namespace mer::project {

/// Loads a .mrp document.
///
/// The format is versioned; readers accept anything at or below
/// kCurrentFormatVersion and upgrade in place. Pre-2024 documents are handled
/// by the compat layer (see src/compat/v1) when MERIDIAN_LEGACY_PROJECTS is on.
class ProjectReader {
public:
    static constexpr int kCurrentFormatVersion = 7;
    static constexpr int kMinimumFormatVersion = 4;

    std::unique_ptr<Project> read(const std::string& path,
                                  util::DiagnosticSink& sink) const;

    /// Reads the "formatVersion" field without parsing the rest.
    static int peekFormatVersion(const std::string& path);
};

} // namespace mer::project
