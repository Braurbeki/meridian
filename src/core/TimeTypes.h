#pragma once
#include <cstdint>
#include <string>

namespace mer::core {

/// Exact rational, used for frame rates and time bases.
struct Rational {
    std::int64_t num = 0;
    std::int64_t den = 1;

    constexpr Rational() = default;
    constexpr Rational(std::int64_t n, std::int64_t d) : num(n), den(d) {}

    double       toDouble() const;
    Rational     reduced() const;
    bool         valid() const { return den != 0; }
    std::string  toString() const;

    bool operator==(const Rational& o) const;
    bool operator!=(const Rational& o) const { return !(*this == o); }
};

/// Frame count on a known rate. Kept separate from wall-clock time so that
/// conversions are always explicit at the call site.
struct FrameSpan {
    std::int64_t frames = 0;
    Rational     rate{25, 1};

    double seconds() const;
    static FrameSpan fromSeconds(double seconds, Rational rate);
};

std::string formatTimecode(std::int64_t frames, Rational rate);
std::string formatDuration(double seconds);

} // namespace mer::core
