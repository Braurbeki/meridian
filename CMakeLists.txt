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

find_package(ffmpeg REQUIRED)

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
