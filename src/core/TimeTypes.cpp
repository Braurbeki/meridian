#include "core/TimeTypes.h"

#include <cmath>
#include <cstdio>
#include <numeric>

namespace mer::core {

double Rational::toDouble() const
{
    return den == 0 ? 0.0 : static_cast<double>(num) / static_cast<double>(den);
}

Rational Rational::reduced() const
{
    if (den == 0) {
        return *this;
    }
    const std::int64_t g = std::gcd(num < 0 ? -num : num, den < 0 ? -den : den);
    if (g == 0) {
        return *this;
    }
    return Rational(num / g, den / g);
}

std::string Rational::toString() const
{
    return std::to_string(num) + "/" + std::to_string(den);
}

bool Rational::operator==(const Rational& o) const
{
    const Rational a = reduced();
    const Rational b = o.reduced();
    return a.num == b.num && a.den == b.den;
}

double FrameSpan::seconds() const
{
    const double r = rate.toDouble();
    return r == 0.0 ? 0.0 : static_cast<double>(frames) / r;
}

FrameSpan FrameSpan::fromSeconds(double seconds, Rational rate)
{
    FrameSpan span;
    span.rate   = rate;
    span.frames = static_cast<std::int64_t>(std::llround(seconds * rate.toDouble()));
    return span;
}

std::string formatTimecode(std::int64_t frames, Rational rate)
{
    const double fps = rate.toDouble();
    if (fps <= 0.0) {
        return "--:--:--:--";
    }
    const std::int64_t whole = static_cast<std::int64_t>(std::llround(fps));
    const std::int64_t ff    = frames % whole;
    const std::int64_t total = frames / whole;
    const std::int64_t ss    = total % 60;
    const std::int64_t mm    = (total / 60) % 60;
    const std::int64_t hh    = total / 3600;

    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02lld:%02lld:%02lld:%02lld",
                  static_cast<long long>(hh), static_cast<long long>(mm),
                  static_cast<long long>(ss), static_cast<long long>(ff));
    return std::string(buf);
}

std::string formatDuration(double seconds)
{
    if (seconds < 0.0) {
        return "--:--";
    }
    const int total = static_cast<int>(seconds);
    char      buf[32];
    if (total >= 3600) {
        std::snprintf(buf, sizeof(buf), "%d:%02d:%02d", total / 3600,
                      (total / 60) % 60, total % 60);
    } else {
        std::snprintf(buf, sizeof(buf), "%d:%02d", total / 60, total % 60);
    }
    return std::string(buf);
}

} // namespace mer::core
