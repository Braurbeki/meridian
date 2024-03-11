#pragma once
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace mer::plugins {

class Plugin {
public:
    virtual ~Plugin() = default;
    virtual std::string identifier() const = 0;
    virtual std::string displayName() const = 0;
    virtual bool        initialize() { return true; }
    virtual void        shutdown() {}
};

using PluginPtr = std::shared_ptr<Plugin>;

/// Registry of optional features, keyed by reverse-dns identifier. Built-in
/// plugins register at static-init time; external ones are loaded from
/// $MERIDIAN_PLUGIN_PATH at startup.
class PluginRegistry {
public:
    using Factory = std::function<PluginPtr()>;

    static PluginRegistry& instance();

    void add(const std::string& identifier, Factory factory);
    PluginPtr create(const std::string& identifier) const;

    std::vector<std::string> identifiers() const;
    bool                     has(const std::string& identifier) const;

private:
    PluginRegistry() = default;

    mutable std::mutex             mutex_;
    std::map<std::string, Factory> factories_;
};

struct PluginRegistrar {
    PluginRegistrar(const std::string& identifier, PluginRegistry::Factory factory)
    {
        PluginRegistry::instance().add(identifier, std::move(factory));
    }
};

} // namespace mer::plugins

#define MER_PLUGIN_CAT_(a, b) a##b
#define MER_PLUGIN_CAT(a, b)  MER_PLUGIN_CAT_(a, b)

#define MER_REGISTER_PLUGIN(identifier, Class)                                 \
    namespace {                                                               \
    const ::mer::plugins::PluginRegistrar MER_PLUGIN_CAT(g_meridian_plugin_,   \
                                                         __LINE__){            \
        identifier, [] { return ::mer::plugins::PluginPtr(new Class()); }};    \
    }
