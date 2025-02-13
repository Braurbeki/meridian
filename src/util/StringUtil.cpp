#include "util/StringUtil.h"

#include <algorithm>
#include <cctype>

namespace mer::util {

std::string trim(const std::string& s)
{
    auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return {};
    }
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

std::string toLower(const std::string& s)
{
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}

bool startsWith(const std::string& s, const std::string& prefix)
{
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size()
        && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> out;
    std::string::size_type   start = 0;
    while (start <= s.size()) {
        auto pos = s.find(delim, start);
        if (pos == std::string::npos) {
            out.push_back(s.substr(start));
            break;
        }
        out.push_back(s.substr(start, pos - start));
        start = pos + 1;
    }
    return out;
}

std::string join(const std::vector<std::string>& parts, const std::string& sep)
{
    std::string out;
    for (std::size_t i = 0; i < parts.size(); ++i) {
        if (i) {
            out += sep;
        }
        out += parts[i];
    }
    return out;
}

std::string normalizeSeparators(const std::string& path)
{
    // Project files store POSIX separators on every platform, but paths that
    // arrive from a Windows host or a v1 document do not.
    std::string out = path;
    std::replace(out.begin(), out.end(), '\\', '/');
    return out;
}

std::string baseName(const std::string& path)
{
    const std::string p   = normalizeSeparators(path);
    const auto        pos = p.find_last_of('/');
    return pos == std::string::npos ? p : p.substr(pos + 1);
}

std::string stemName(const std::string& path)
{
    const std::string base = baseName(path);
    const auto        dot  = base.find_last_of('.');
    return dot == std::string::npos ? base : base.substr(0, dot);
}

std::string extension(const std::string& path)
{
    const std::string base = baseName(path);
    const auto        dot  = base.find_last_of('.');
    return dot == std::string::npos ? std::string() : base.substr(dot + 1);
}

std::string parentPath(const std::string& path)
{
    const std::string p   = normalizeSeparators(path);
    const auto        pos = p.find_last_of('/');
    return pos == std::string::npos ? std::string() : p.substr(0, pos);
}

} // namespace mer::util
