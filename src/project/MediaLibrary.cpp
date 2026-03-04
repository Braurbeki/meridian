#include "project/MediaLibrary.h"

#include "util/Log.h"
#include "util/StringUtil.h"

#include <filesystem>

namespace mer::project {
namespace {

bool fileExists(const std::string& path)
{
    if (path.empty()) {
        return false;
    }
    std::error_code ec;
    return std::filesystem::exists(std::filesystem::u8path(path), ec);
}

} // namespace

core::MediaSourcePtr MediaLibrary::add(core::MediaSourcePtr source)
{
    if (!source) {
        return nullptr;
    }
    byId_[source->id()] = source;
    return source;
}

core::MediaSourcePtr MediaLibrary::find(const util::Uuid& id) const
{
    const auto it = byId_.find(id);
    return it == byId_.end() ? nullptr : it->second;
}

core::MediaSourcePtr MediaLibrary::findByPath(const std::string& path) const
{
    const std::string normalized = util::normalizeSeparators(path);
    for (const auto& kv : byId_) {
        if (util::normalizeSeparators(kv.second->path()) == normalized) {
            return kv.second;
        }
    }
    return nullptr;
}

std::vector<core::MediaSourcePtr> MediaLibrary::all() const
{
    std::vector<core::MediaSourcePtr> out;
    out.reserve(byId_.size());
    for (const auto& kv : byId_) {
        out.push_back(kv.second);
    }
    return out;
}

std::string MediaLibrary::resolvePath(const std::string& recordedPath) const
{
    const std::string normalized = util::normalizeSeparators(recordedPath);
    if (normalized.empty()) {
        return {};
    }
    if (normalized[0] == '/' || fileExists(normalized)) {
        return normalized;
    }
    if (projectRoot_.empty()) {
        return normalized;
    }
    return util::normalizeSeparators(projectRoot_ + "/" + normalized);
}

void MediaLibrary::refreshAll(util::DiagnosticSink& sink)
{
    const core::MediaProbe probe;
    int                    offline = 0;

    for (const auto& kv : byId_) {
        const core::MediaSourcePtr& source = kv.second;
        source->setResolvedPath(resolvePath(source->path()));
        if (!probe.probe(*source, sink)) {
            ++offline;
        }
    }

    MER_INFO("media") << "refreshed " << byId_.size() << " source(s), " << offline
                      << " offline";
}

} // namespace mer::project
