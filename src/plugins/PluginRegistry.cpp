#include "plugins/PluginRegistry.h"

#include "util/Log.h"

namespace mer::plugins {

PluginRegistry& PluginRegistry::instance()
{
    static PluginRegistry registry;
    return registry;
}

void PluginRegistry::add(const std::string& identifier, Factory factory)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MER_DEBUG("plugins") << "register '" << identifier << "'";
    factories_[identifier] = std::move(factory);
}

PluginPtr PluginRegistry::create(const std::string& identifier) const
{
    Factory factory;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = factories_.find(identifier);
        if (it == factories_.end()) {
            return nullptr;
        }
        factory = it->second;
    }
    return factory();
}

std::vector<std::string> PluginRegistry::identifiers() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string>    out;
    out.reserve(factories_.size());
    for (const auto& kv : factories_) {
        out.push_back(kv.first);
    }
    return out;
}

bool PluginRegistry::has(const std::string& identifier) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return factories_.count(identifier) > 0;
}

} // namespace mer::plugins
