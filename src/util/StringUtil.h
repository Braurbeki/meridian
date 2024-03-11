#pragma once
#include <string>
#include <vector>

namespace mer::util {

std::string              trim(const std::string& s);
std::string              toLower(const std::string& s);
bool                     startsWith(const std::string& s, const std::string& prefix);
bool                     endsWith(const std::string& s, const std::string& suffix);
std::vector<std::string> split(const std::string& s, char delim);
std::string              join(const std::vector<std::string>& parts, const std::string& sep);

/// Filesystem-flavoured helpers. These predate std::filesystem in this
/// codebase and are kept because the project format stores POSIX-style
/// paths regardless of host platform.
std::string baseName(const std::string& path);
std::string stemName(const std::string& path);
std::string extension(const std::string& path);
std::string parentPath(const std::string& path);
std::string normalizeSeparators(const std::string& path);

} // namespace mer::util
