option(MERIDIAN_BUILD_TESTS      "Build unit tests"                 ON)

# auto | ON | OFF -- see the FFmpeg block in the top-level CMakeLists.
set(MERIDIAN_WITH_FFMPEG "auto" CACHE STRING "Use FFmpeg for media probing")
set_property(CACHE MERIDIAN_WITH_FFMPEG PROPERTY STRINGS auto ON OFF)
option(MERIDIAN_ENABLE_TELEMETRY "Enable usage telemetry hooks"     OFF)
option(MERIDIAN_LEGACY_PROJECTS  "Support pre-2024 project formats" ON)
option(MERIDIAN_STRICT_WARNINGS  "Treat selected warnings as errors" OFF)

set(MERIDIAN_LOG_LEVEL "info" CACHE STRING "Compile-time log floor: trace|debug|info|warn|error")
set_property(CACHE MERIDIAN_LOG_LEVEL PROPERTY STRINGS trace debug info warn error)
