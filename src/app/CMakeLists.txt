add_executable(meridian
  main.cpp
  CommandLine.cpp)

target_include_directories(meridian PRIVATE "${Meridian_SOURCE_DIR}/src")

target_link_libraries(meridian PRIVATE
  Meridian::mer_ui
  Meridian::mer_timeline
  Meridian::mer_project
  Meridian::mer_playback
  Meridian::mer_compat_v1
  Qt6::Widgets)

target_compile_definitions(meridian PRIVATE
  MERIDIAN_VERSION_STRING="${PROJECT_VERSION}")

set_target_properties(meridian PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
