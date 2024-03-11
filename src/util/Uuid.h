#pragma once
#include <cstdint>
#include <string>

namespace mer::util {

/// 128-bit identifier used for every persisted entity (clips, segments,
/// tracks, bins). Stable across save/load; regenerated only on duplicate.
class Uuid {
public:
    Uuid() = default;
    Uuid(std::uint64_t hi, std::uint64_t lo) : hi_(hi), lo_(lo) {}

    static Uuid generate();
    static Uuid fromString(const std::string& text);
    static Uuid nil() { return Uuid(); }

    std::string toString() const;
    bool        isNil() const { return hi_ == 0 && lo_ == 0; }

    bool operator==(const Uuid& o) const { return hi_ == o.hi_ && lo_ == o.lo_; }
    bool operator!=(const Uuid& o) const { return !(*this == o); }
    bool operator<(const Uuid& o) const
    {
        return hi_ != o.hi_ ? hi_ < o.hi_ : lo_ < o.lo_;
    }

    std::uint64_t high() const { return hi_; }
    std::uint64_t low() const { return lo_; }

private:
    std::uint64_t hi_ = 0;
    std::uint64_t lo_ = 0;
};

} // namespace mer::util

namespace std {
template <>
struct hash<::mer::util::Uuid> {
    size_t operator()(const ::mer::util::Uuid& u) const noexcept
    {
        return static_cast<size_t>(u.high() ^ (u.low() * 1099511628211ull));
    }
};
} // namespace std
