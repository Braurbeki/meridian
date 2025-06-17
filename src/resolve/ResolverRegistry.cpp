#include "resolve/ResolverRegistry.h"

#include "util/Log.h"

namespace mer::resolve {

ResolverRegistry& ResolverRegistry::instance()
{
    static ResolverRegistry registry;
    return registry;
}

void ResolverRegistry::add(std::string key, Factory factory)
{
    std::lock_guard<std::mutex> lock(mutex_);
    MER_DEBUG("resolve") << "register '" << key << "'";
    factories_[std::move(key)] = std::move(factory);
}

std::string ResolverRegistry::resolve(const std::string& key,
                                      const ResolveContext& ctx) const
{
    Factory factory;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = factories_.find(key);
        if (it == factories_.end()) {
            MER_WARN("resolve") << "no resolver registered for '" << key << "'";
            return {};
        }
        factory = it->second;
    }

    const ResolverPtr resolver = factory();
    std::string       out      = resolver ? resolver->resolve(ctx) : std::string();
    MER_TRACE("resolve") << key << " -> '" << out << "'";
    return out;
}

bool ResolverRegistry::has(const std::string& key) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return factories_.count(key) > 0;
}

std::vector<std::string> ResolverRegistry::keys() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string>    out;
    out.reserve(factories_.size());
    for (const auto& kv : factories_) {
        out.push_back(kv.first);
    }
    return out;
}

} // namespace mer::resolve
