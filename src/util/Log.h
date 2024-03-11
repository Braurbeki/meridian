#pragma once
#include <string>
#include <sstream>

#define MER_LOG_TRACE 0
#define MER_LOG_DEBUG 1
#define MER_LOG_INFO  2
#define MER_LOG_WARN  3
#define MER_LOG_ERROR 4

#ifndef MERIDIAN_LOG_FLOOR
#  define MERIDIAN_LOG_FLOOR MER_LOG_INFO
#endif

namespace mer::util {

enum class LogLevel { Trace = 0, Debug, Info, Warn, Error };

/// Channels let a developer raise verbosity for one subsystem without
/// drowning in the rest. Set MERIDIAN_LOG_CHANNELS=timeline,resolve at runtime.
void enableChannel(const std::string& channel);
bool channelEnabled(const std::string& channel);
void initLoggingFromEnv();

void writeLog(LogLevel level, const char* channel, const char* file, int line,
              const std::string& message);

namespace detail {
struct LogStream {
    LogLevel    level;
    const char* channel;
    const char* file;
    int         line;
    std::ostringstream os;

    ~LogStream() { writeLog(level, channel, file, line, os.str()); }

    template <typename T>
    LogStream& operator<<(const T& v) { os << v; return *this; }
};
} // namespace detail

} // namespace mer::util

#define MER_LOG(lvl, chan)                                                     \
    if (MER_LOG_##lvl < MERIDIAN_LOG_FLOOR) {                                  \
    } else                                                                     \
        ::mer::util::detail::LogStream {                                       \
            ::mer::util::LogLevel(MER_LOG_##lvl), chan, __FILE__, __LINE__, {} \
        }

#define MER_TRACE(chan) MER_LOG(TRACE, chan)
#define MER_DEBUG(chan) MER_LOG(DEBUG, chan)
#define MER_INFO(chan)  MER_LOG(INFO, chan)
#define MER_WARN(chan)  MER_LOG(WARN, chan)
#define MER_ERROR(chan) MER_LOG(ERROR, chan)
