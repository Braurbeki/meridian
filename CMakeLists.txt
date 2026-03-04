cmake_minimum_required(VERSION 3.24)
project(Meridian VERSION 2026.12.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MeridianOptions)
include(MeridianTargets)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets)

# FFmpeg is optional. With it we read duration, streams and container tags off
# the media; without it the prober only establishes that the file is there.
# Nothing else in the application changes.
if(MERIDIAN_WITH_FFMPEG STREQUAL "auto")
  find_package(FFmpeg QUIET COMPONENTS avformat avcodec avutil swscale)
elseif(MERIDIAN_WITH_FFMPEG)
  find_package(FFmpeg REQUIRED COMPONENTS avformat avcodec avutil swscale)
endif()

if(FFmpeg_FOUND)
  message(STATUS "FFmpeg: enabled")
else()
  message(STATUS "FFmpeg: not used (media will be probed for presence only)")
endif()

# --- module graph ---------------------------------------------------------
add_subdirectory(src/util)
add_subdirectory(src/core)
add_subdirectory(src/resolve)
add_subdirectory(src/project)
add_subdirectory(src/timeline)
add_subdirectory(src/playback)
add_subdirectory(src/compat/v1)
add_subdirectory(src/plugins)

# Qt automatic tooling, kept together with the widget layer that needs it.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src/ui)
add_subdirectory(src/app)

if(MERIDIAN_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

message(STATUS "Meridian ${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
