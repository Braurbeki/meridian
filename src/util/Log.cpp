#include "util/Log.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mutex>
#include <set>
#include <string>

namespace mer::util {
namespace {

std::mutex           g_mutex;
std::set<std::string> g_channels;
bool                 g_allChannels = false;

const char* levelTag(LogLevel l)
{
    switch (l) {
    case LogLevel::Trace: return "TRC";
    case LogLevel::Debug: return "DBG";
    case LogLevel::Info:  return "INF";
    case LogLevel::Warn:  return "WRN";
    case LogLevel::Error: return "ERR";
    }
    return "???";
}

const char* basename(const char* path)
{
    const char* slash = std::strrchr(path, '/');
    return slash ? slash + 1 : path;
}

} // namespace

void enableChannel(const std::string& channel)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (channel == "*") {
        g_allChannels = true;
    } else {
        g_channels.insert(channel);
    }
}

bool channelEnabled(const std::string& channel)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_allChannels || g_channels.count(channel) > 0;
}

void initLoggingFromEnv()
{
    const char* env = std::getenv("MERIDIAN_LOG_CHANNELS");
    if (!env) {
        return;
    }
    std::string    spec(env);
    std::string::size_type start = 0;
    while (start <= spec.size()) {
        auto comma = spec.find(',', start);
        if (comma == std::string::npos) {
            comma = spec.size();
        }
        auto token = spec.substr(start, comma - start);
        if (!token.empty()) {
            enableChannel(token);
        }
        start = comma + 1;
    }
}

void writeLog(LogLevel level, const char* channel, const char* file, int line,
              const std::string& message)
{
    // Trace and debug are opt-in per channel; anything louder always prints.
    if (level < LogLevel::Info && !channelEnabled(channel)) {
        return;
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    std::ostream& out = (level >= LogLevel::Warn) ? std::cerr : std::cout;
    out << '[' << levelTag(level) << "][" << channel << "] " << message
        << "  (" << basename(file) << ':' << line << ")\n";
}

} // namespace mer::util
