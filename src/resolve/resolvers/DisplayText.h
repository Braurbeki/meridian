#pragma once
#include "resolve/ResolveContext.h"

#include <string>

namespace mer::resolve {

/// Applies the context's display constraints. Kept in one place so every
/// resolver truncates identically -- panels used to disagree by a character.
std::string applyDisplayLimits(std::string text, const ResolveContext& ctx);

} // namespace mer::resolve
