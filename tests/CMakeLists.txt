# Minimal assert-based harness; we deliberately have no external test
# dependency so that a fresh checkout builds with nothing but Qt and FFmpeg.
add_library(mer_testmain STATIC TestMain.cpp)
target_include_directories(mer_testmain PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

function(meridian_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PRIVATE mer_testmain ${ARGN})
  target_include_directories(${name} PRIVATE "${CMAKE_SOURCE_DIR}/src")
  add_test(NAME ${name} COMMAND ${name})
endfunction()

meridian_test(StringUtilTest   Meridian::mer_util)
meridian_test(MetadataTest     Meridian::mer_core)
meridian_test(FrameCacheTest   Meridian::mer_playback)
meridian_test(TimelineBuildTest Meridian::mer_timeline)
