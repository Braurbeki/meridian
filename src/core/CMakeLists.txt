meridian_module(mer_core
  SOURCES
    MetadataValue.cpp
    MetadataDict.cpp
    MediaSource.cpp
    MediaProbe.cpp
    TimeTypes.cpp
  PUBLIC_DEPS
    Meridian::mer_util)

target_link_libraries(mer_core PRIVATE
  ffmpeg::avformat ffmpeg::avcodec ffmpeg::avutil)
