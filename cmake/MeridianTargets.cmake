# Shared configuration applied to every first-party target.
function(meridian_module name)
  cmake_parse_arguments(ARG "" "" "SOURCES;PUBLIC_DEPS;PRIVATE_DEPS" ${ARGN})

  add_library(${name} STATIC ${ARG_SOURCES})
  add_library(Meridian::${name} ALIAS ${name})

  target_include_directories(${name}
    PUBLIC  "${CMAKE_SOURCE_DIR}/src"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

  if(ARG_PUBLIC_DEPS)
    target_link_libraries(${name} PUBLIC ${ARG_PUBLIC_DEPS})
  endif()
  if(ARG_PRIVATE_DEPS)
    target_link_libraries(${name} PRIVATE ${ARG_PRIVATE_DEPS})
  endif()

  if(MSVC)
    # Sources are UTF-8; without this MSVC guesses the local codepage.
    target_compile_options(${name} PRIVATE /utf-8)
    target_compile_definitions(${name} PRIVATE _CRT_SECURE_NO_WARNINGS
                                               NOMINMAX WIN32_LEAN_AND_MEAN)
  endif()

  if(MERIDIAN_STRICT_WARNINGS AND NOT MSVC)
    target_compile_options(${name} PRIVATE -Wall -Wextra -Wno-unused-parameter)
  endif()

  if(FFmpeg_FOUND)
    target_compile_definitions(${name} PUBLIC MERIDIAN_WITH_FFMPEG=1)
  endif()

  string(TOUPPER "${MERIDIAN_LOG_LEVEL}" _lvl)
  target_compile_definitions(${name} PUBLIC MERIDIAN_LOG_FLOOR=MER_LOG_${_lvl})

  if(MERIDIAN_ENABLE_TELEMETRY)
    target_compile_definitions(${name} PUBLIC MERIDIAN_TELEMETRY=1)
  endif()
  if(MERIDIAN_LEGACY_PROJECTS)
    target_compile_definitions(${name} PUBLIC MERIDIAN_LEGACY_PROJECTS=1)
  endif()
endfunction()

# Targets whose objects register themselves at static-init time and are never
# referenced directly. Without whole-archive linking the linker drops the
# translation unit and the registration silently never happens.
function(meridian_link_whole target)
  foreach(dep ${ARGN})
    target_link_libraries(${target} PUBLIC
      "$<LINK_LIBRARY:WHOLE_ARCHIVE,${dep}>")
  endforeach()
endfunction()
