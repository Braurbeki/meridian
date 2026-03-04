meridian_module(mer_core
  SOURCES
    MetadataValue.cpp
    MetadataDict.cpp
    MediaSource.cpp
    MediaProbe.cpp
    TimeTypes.cpp
  PUBLIC_DEPS
    Meridian::mer_util)

if(FFmpeg_FOUND)
  target_link_libraries(mer_core PRIVATE
    FFmpeg::avformat FFmpeg::avcodec FFmpeg::avutil)
endif()
