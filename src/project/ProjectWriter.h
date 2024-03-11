#pragma once
#include "project/Project.h"
#include "util/Diagnostics.h"

#include <string>

namespace mer::project {

/// Serialises a Project to .mrp at kCurrentFormatVersion. Writes to a
/// temporary file and renames, so an interrupted save cannot corrupt the
/// document on disk.
class ProjectWriter {
public:
    bool write(const Project& project, const std::string& path,
               util::DiagnosticSink& sink) const;
};

} // namespace mer::project
