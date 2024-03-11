#include "util/Diagnostics.h"

#include "util/Log.h"

#include <algorithm>

namespace mer::util {

void DiagnosticSink::add(Severity sev, std::string code, std::string message,
                         std::string context)
{
    if (sev == Severity::Error) {
        MER_WARN("diag") << code << ": " << message;
    } else {
        MER_DEBUG("diag") << code << ": " << message;
    }
    entries_.push_back(Diagnostic{sev, std::move(code), std::move(message),
                                  std::move(context)});
}

bool DiagnosticSink::hasErrors() const
{
    return std::any_of(entries_.begin(), entries_.end(),
                       [](const Diagnostic& d) { return d.severity == Severity::Error; });
}

} // namespace mer::util
