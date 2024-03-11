#pragma once
#include <cstdint>
#include <string>

namespace mer::core {

/// A single metadata field read from a container, a sidecar, or set by the
/// user. Values are weakly typed because the sources we ingest are: broadcast
/// containers routinely store numbers as strings and vice versa.
class MetadataValue {
public:
    enum class Type { None, String, Int, Real, Bool };

    MetadataValue() = default;
    static MetadataValue fromString(std::string v);
    static MetadataValue fromInt(std::int64_t v);
    static MetadataValue fromReal(double v);
    static MetadataValue fromBool(bool v);

    Type type() const { return type_; }

    /// True when the field was present in the source at all.
    ///
    /// Note this is presence, not usefulness: a container that writes an
    /// empty TITLE tag has a present field whose content is the empty
    /// string. Callers that need "has something worth showing" want
    /// hasContent() instead.
    bool isSet() const { return type_ != Type::None; }

    /// True when the field is present *and* carries a non-empty value.
    bool hasContent() const;

    std::string  asString() const;
    std::int64_t asInt(std::int64_t fallback = 0) const;
    double       asReal(double fallback = 0.0) const;
    bool         asBool(bool fallback = false) const;

    bool operator==(const MetadataValue& o) const;
    bool operator!=(const MetadataValue& o) const { return !(*this == o); }

private:
    Type         type_ = Type::None;
    std::string  str_;
    std::int64_t int_ = 0;
    double       real_ = 0.0;
    bool         bool_ = false;
};

} // namespace mer::core
