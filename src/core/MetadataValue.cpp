#include "core/MetadataValue.h"

#include "util/StringUtil.h"

#include <cstdlib>
#include <sstream>

namespace mer::core {

MetadataValue MetadataValue::fromString(std::string v)
{
    MetadataValue m;
    m.type_ = Type::String;
    m.str_  = std::move(v);
    return m;
}

MetadataValue MetadataValue::fromInt(std::int64_t v)
{
    MetadataValue m;
    m.type_ = Type::Int;
    m.int_  = v;
    return m;
}

MetadataValue MetadataValue::fromReal(double v)
{
    MetadataValue m;
    m.type_ = Type::Real;
    m.real_ = v;
    return m;
}

MetadataValue MetadataValue::fromBool(bool v)
{
    MetadataValue m;
    m.type_ = Type::Bool;
    m.bool_ = v;
    return m;
}

bool MetadataValue::hasContent() const
{
    switch (type_) {
    case Type::None:   return false;
    case Type::String: return !util::trim(str_).empty();
    default:           return true;
    }
}

std::string MetadataValue::asString() const
{
    switch (type_) {
    case Type::None:   return {};
    case Type::String: return str_;
    case Type::Int:    return std::to_string(int_);
    case Type::Bool:   return bool_ ? "true" : "false";
    case Type::Real: {
        std::ostringstream os;
        os << real_;
        return os.str();
    }
    }
    return {};
}

std::int64_t MetadataValue::asInt(std::int64_t fallback) const
{
    switch (type_) {
    case Type::Int:    return int_;
    case Type::Real:   return static_cast<std::int64_t>(real_);
    case Type::Bool:   return bool_ ? 1 : 0;
    case Type::String: {
        if (str_.empty()) {
            return fallback;
        }
        char* end = nullptr;
        const long long parsed = std::strtoll(str_.c_str(), &end, 10);
        return (end && *end == '\0') ? static_cast<std::int64_t>(parsed) : fallback;
    }
    case Type::None:   return fallback;
    }
    return fallback;
}

double MetadataValue::asReal(double fallback) const
{
    switch (type_) {
    case Type::Real:   return real_;
    case Type::Int:    return static_cast<double>(int_);
    case Type::Bool:   return bool_ ? 1.0 : 0.0;
    case Type::String: {
        if (str_.empty()) {
            return fallback;
        }
        char* end = nullptr;
        const double parsed = std::strtod(str_.c_str(), &end);
        return (end && *end == '\0') ? parsed : fallback;
    }
    case Type::None:   return fallback;
    }
    return fallback;
}

bool MetadataValue::asBool(bool fallback) const
{
    switch (type_) {
    case Type::Bool: return bool_;
    case Type::Int:  return int_ != 0;
    case Type::Real: return real_ != 0.0;
    case Type::String: {
        const std::string lowered = util::toLower(util::trim(str_));
        if (lowered == "1" || lowered == "true" || lowered == "yes") return true;
        if (lowered == "0" || lowered == "false" || lowered == "no") return false;
        return fallback;
    }
    case Type::None: return fallback;
    }
    return fallback;
}

bool MetadataValue::operator==(const MetadataValue& o) const
{
    if (type_ != o.type_) {
        return false;
    }
    switch (type_) {
    case Type::None:   return true;
    case Type::String: return str_ == o.str_;
    case Type::Int:    return int_ == o.int_;
    case Type::Real:   return real_ == o.real_;
    case Type::Bool:   return bool_ == o.bool_;
    }
    return false;
}

} // namespace mer::core
