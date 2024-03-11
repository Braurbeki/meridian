#pragma once
#include "core/MetadataValue.h"

#include <map>
#include <string>
#include <vector>

namespace mer::core {

/// Case-insensitive metadata bag. Container tags arrive with wildly
/// inconsistent casing (TITLE, Title, title) so lookups normalise.
class MetadataDict {
public:
    void set(const std::string& key, MetadataValue value);
    void setString(const std::string& key, std::string value)
    {
        set(key, MetadataValue::fromString(std::move(value)));
    }
    void remove(const std::string& key);
    void clear() { entries_.clear(); }

    /// Returns a None-typed value when the key is absent.
    MetadataValue get(const std::string& key) const;

    bool contains(const std::string& key) const;
    bool empty() const { return entries_.empty(); }
    std::size_t size() const { return entries_.size(); }

    std::vector<std::string> keys() const;

    /// Copies every entry from `other`, overwriting on collision.
    void merge(const MetadataDict& other);

    std::map<std::string, MetadataValue>::const_iterator begin() const
    {
        return entries_.begin();
    }
    std::map<std::string, MetadataValue>::const_iterator end() const
    {
        return entries_.end();
    }

private:
    static std::string normalizeKey(const std::string& key);

    std::map<std::string, MetadataValue> entries_;
};

} // namespace mer::core
