#pragma once
#include "resolve/ResolveContext.h"

#include <memory>
#include <string>

namespace mer::resolve {

/// A named, stateless computation that turns a ResolveContext into a display
/// string. Registered by key so that panels, exporters and the timeline all
/// agree on what a given field means without depending on each other.
class Resolver {
public:
    virtual ~Resolver() = default;

    virtual std::string resolve(const ResolveContext& ctx) const = 0;

    /// Short description shown in Preferences > Columns.
    virtual std::string description() const { return {}; }
};

using ResolverPtr = std::unique_ptr<Resolver>;

} // namespace mer::resolve
