#include "util/Uuid.h"

#include <cstdio>
#include <random>

namespace mer::util {
namespace {

std::mt19937_64& engine()
{
    static thread_local std::mt19937_64 eng{std::random_device{}()};
    return eng;
}

int hexValue(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

} // namespace

Uuid Uuid::generate()
{
    std::uniform_int_distribution<std::uint64_t> dist;
    return Uuid(dist(engine()), dist(engine()));
}

std::string Uuid::toString() const
{
    char buf[37];
    std::snprintf(buf, sizeof(buf), "%08x-%04x-%04x-%04x-%012llx",
                  static_cast<unsigned>(hi_ >> 32),
                  static_cast<unsigned>((hi_ >> 16) & 0xffff),
                  static_cast<unsigned>(hi_ & 0xffff),
                  static_cast<unsigned>(lo_ >> 48),
                  static_cast<unsigned long long>(lo_ & 0xffffffffffffull));
    return std::string(buf);
}

Uuid Uuid::fromString(const std::string& text)
{
    std::uint64_t hi = 0, lo = 0;
    int           digits = 0;
    for (char c : text) {
        if (c == '-') {
            continue;
        }
        const int v = hexValue(c);
        if (v < 0) {
            return Uuid::nil();
        }
        if (digits < 16) {
            hi = (hi << 4) | static_cast<std::uint64_t>(v);
        } else if (digits < 32) {
            lo = (lo << 4) | static_cast<std::uint64_t>(v);
        }
        ++digits;
    }
    return digits == 32 ? Uuid(hi, lo) : Uuid::nil();
}

} // namespace mer::util
