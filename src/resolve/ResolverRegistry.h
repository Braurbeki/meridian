#pragma once
#include "resolve/Resolver.h"

#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace mer::resolve {

/// Process-wide registry of field resolvers, keyed by "<domain>.<field>".
///
/// Entries are installed at static-initialisation time by the
/// MER_REGISTER_RESOLVER macro, so there is no central list of keys to keep
/// in sync -- adding a .cpp to the module is enough. Run with
/// MERIDIAN_LOG_CHANNELS=resolve to see what registered.
class ResolverRegistry {
public:
    using Factory = std::function<ResolverPtr()>;

    static ResolverRegistry& instance();

    void add(std::string key, Factory factory);

    /// Returns an empty string when no resolver is registered for `key`.
    std::string resolve(const std::string& key, const ResolveContext& ctx) const;

    bool                     has(const std::string& key) const;
    std::vector<std::string> keys() const;

private:
    ResolverRegistry() = default;

    mutable std::mutex             mutex_;
    std::map<std::string, Factory> factories_;
};

/// Static-init helper. Construct one at namespace scope to register.
struct Registrar {
    Registrar(std::string key, ResolverRegistry::Factory factory)
    {
        ResolverRegistry::instance().add(std::move(key), std::move(factory));
    }
};

} // namespace mer::resolve

#define MER_RESOLVER_CAT_(a, b) a##b
#define MER_RESOLVER_CAT(a, b)  MER_RESOLVER_CAT_(a, b)

/// Registers `Class` under the key "<domain>.<field>".
///
///     MER_REGISTER_RESOLVER(track, header, TrackHeaderResolver)
///
/// installs a resolver reachable as "track.header".
#define MER_REGISTER_RESOLVER(domain, field, Class)                            \
    namespace {                                                                \
    const ::mer::resolve::Registrar MER_RESOLVER_CAT(g_meridian_registrar_,     \
                                                     __LINE__){                 \
        #domain "." #field,                                                     \
        [] { return ::mer::resolve::ResolverPtr(new Class()); }};               \
    }
