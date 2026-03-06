meridian_module(mer_playback
  SOURCES
    Clock.cpp
    FrameCache.cpp
    VideoDecoder.cpp
    FrameSource.cpp
    PlaybackEngine.cpp
  PUBLIC_DEPS
    Meridian::mer_timeline)

target_link_libraries(mer_playback PRIVATE
  ffmpeg::avformat ffmpeg::avcodec ffmpeg::avutil ffmpeg::swscale)
