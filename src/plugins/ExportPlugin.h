#pragma once
#include "plugins/PluginRegistry.h"
#include "timeline/Timeline.h"

#include <string>

namespace mer::plugins {

/// Plugins that can turn a built timeline into a deliverable file.
class ExportPlugin : public Plugin {
public:
    virtual std::string fileExtension() const = 0;
    virtual bool exportTimeline(const timeline::Timeline& timeline,
                                const std::string& outputPath) = 0;
};

} // namespace mer::plugins
