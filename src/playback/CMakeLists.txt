meridian_module(mer_playback
  SOURCES
    Clock.cpp
    FrameCache.cpp
    VideoDecoder.cpp
    FrameSource.cpp
    PlaybackEngine.cpp
  PUBLIC_DEPS
    Meridian::mer_timeline)

if(FFmpeg_FOUND)
  target_link_libraries(mer_playback PRIVATE
    FFmpeg::avformat FFmpeg::avcodec FFmpeg::avutil FFmpeg::swscale)
endif()
