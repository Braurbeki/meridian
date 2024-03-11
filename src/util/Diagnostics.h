#pragma once
#include <string>
#include <vector>

namespace mer::util {

enum class Severity { Info, Warning, Error };

struct Diagnostic {
    Severity    severity = Severity::Info;
    std::string code;     ///< stable machine-readable tag, e.g. "media.offline"
    std::string message;  ///< human-readable, shown in the Console panel
    std::string context;  ///< optional entity id or path
};

/// Collects non-fatal problems encountered while loading a project or
/// building a timeline, so the UI can surface them all at once rather than
/// failing on the first one.
class DiagnosticSink {
public:
    void add(Severity sev, std::string code, std::string message,
             std::string context = {});

    void info(std::string code, std::string message, std::string context = {})
    {
        add(Severity::Info, std::move(code), std::move(message), std::move(context));
    }
    void warn(std::string code, std::string message, std::string context = {})
    {
        add(Severity::Warning, std::move(code), std::move(message), std::move(context));
    }
    void error(std::string code, std::string message, std::string context = {})
    {
        add(Severity::Error, std::move(code), std::move(message), std::move(context));
    }

    const std::vector<Diagnostic>& entries() const { return entries_; }
    bool                           hasErrors() const;
    void                           clear() { entries_.clear(); }
    std::size_t                    size() const { return entries_.size(); }

private:
    std::vector<Diagnostic> entries_;
};

} // namespace mer::util
