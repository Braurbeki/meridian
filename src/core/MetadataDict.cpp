#include "core/MetadataDict.h"

#include "util/StringUtil.h"

namespace mer::core {

std::string MetadataDict::normalizeKey(const std::string& key)
{
    return util::toLower(util::trim(key));
}

void MetadataDict::set(const std::string& key, MetadataValue value)
{
    entries_[normalizeKey(key)] = std::move(value);
}

void MetadataDict::remove(const std::string& key)
{
    entries_.erase(normalizeKey(key));
}

MetadataValue MetadataDict::get(const std::string& key) const
{
    const auto it = entries_.find(normalizeKey(key));
    return it == entries_.end() ? MetadataValue() : it->second;
}

bool MetadataDict::contains(const std::string& key) const
{
    return entries_.count(normalizeKey(key)) > 0;
}

std::vector<std::string> MetadataDict::keys() const
{
    std::vector<std::string> out;
    out.reserve(entries_.size());
    for (const auto& kv : entries_) {
        out.push_back(kv.first);
    }
    return out;
}

void MetadataDict::merge(const MetadataDict& other)
{
    for (const auto& kv : other.entries_) {
        entries_[kv.first] = kv.second;
    }
}

} // namespace mer::core
