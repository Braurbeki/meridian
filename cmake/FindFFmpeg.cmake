# Minimal FFmpeg locator. We do not vendor FFmpeg; the build expects the
# development headers to be present on the system (see README).
include(FindPackageHandleStandardArgs)

set(_ff_found_components TRUE)
foreach(_comp ${FFmpeg_FIND_COMPONENTS})
  find_path(FFmpeg_${_comp}_INCLUDE_DIR
    NAMES "lib${_comp}/${_comp}.h"
    PATH_SUFFIXES ffmpeg)
  find_library(FFmpeg_${_comp}_LIBRARY NAMES ${_comp})

  if(FFmpeg_${_comp}_INCLUDE_DIR AND FFmpeg_${_comp}_LIBRARY)
    if(NOT TARGET FFmpeg::${_comp})
      add_library(FFmpeg::${_comp} UNKNOWN IMPORTED)
      set_target_properties(FFmpeg::${_comp} PROPERTIES
        IMPORTED_LOCATION "${FFmpeg_${_comp}_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_${_comp}_INCLUDE_DIR}")
    endif()
  else()
    set(_ff_found_components FALSE)
    if(NOT FFmpeg_FIND_QUIETLY)
      message(STATUS "FFmpeg: missing component '${_comp}'")
    endif()
  endif()
endforeach()

find_package_handle_standard_args(FFmpeg REQUIRED_VARS _ff_found_components)
