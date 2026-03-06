#include "playback/VideoDecoder.h"

#include "util/Log.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <algorithm>
#include <cmath>

namespace mer::playback {

struct VideoDecoder::Impl {
    std::string      path;
    AVFormatContext* format  = nullptr;
    AVCodecContext*  codec   = nullptr;
    SwsContext*      scaler  = nullptr;
    AVFrame*         frame   = nullptr;
    AVFrame*         rgb     = nullptr;
    AVPacket*        packet  = nullptr;
    int              stream  = -1;
    int              scaledW = 0;
    int              scaledH = 0;

    ~Impl()
    {
        if (scaler)  sws_freeContext(scaler);
        if (rgb)     av_frame_free(&rgb);
        if (frame)   av_frame_free(&frame);
        if (packet)  av_packet_free(&packet);
        if (codec)   avcodec_free_context(&codec);
        if (format)  avformat_close_input(&format);
    }

    /// (Re)builds the scaler when the requested output width changes.
    bool ensureScaler(int maxWidth)
    {
        const double aspect = static_cast<double>(codec->height) / codec->width;
        int w = std::min(maxWidth, codec->width);
        w -= (w % 2);
        int h = static_cast<int>(std::lround(w * aspect));
        h -= (h % 2);
        if (w <= 0 || h <= 0) {
            return false;
        }
        if (scaler && w == scaledW && h == scaledH) {
            return true;
        }

        if (scaler) {
            sws_freeContext(scaler);
            scaler = nullptr;
        }
        scaler = sws_getContext(codec->width, codec->height, codec->pix_fmt, w, h,
                                AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr,
                                nullptr);
        if (!scaler) {
            return false;
        }

        if (rgb) {
            av_frame_free(&rgb);
        }
        rgb = av_frame_alloc();
        rgb->format = AV_PIX_FMT_RGB24;
        rgb->width  = w;
        rgb->height = h;
        if (av_frame_get_buffer(rgb, 32) < 0) {
            return false;
        }

        scaledW = w;
        scaledH = h;
        return true;
    }
};

VideoDecoder::VideoDecoder(std::string path) : impl_(std::make_unique<Impl>())
{
    impl_->path = std::move(path);
}

VideoDecoder::~VideoDecoder() = default;

bool VideoDecoder::isOpen() const { return impl_ && impl_->codec != nullptr; }

int    VideoDecoder::nativeWidth() const  { return isOpen() ? impl_->codec->width : 0; }
int    VideoDecoder::nativeHeight() const { return isOpen() ? impl_->codec->height : 0; }

double VideoDecoder::duration() const
{
    if (!impl_ || !impl_->format || impl_->format->duration == AV_NOPTS_VALUE) {
        return 0.0;
    }
    return static_cast<double>(impl_->format->duration) / AV_TIME_BASE;
}

bool VideoDecoder::open(util::DiagnosticSink& sink)
{
    if (isOpen()) {
        return true;
    }

    if (avformat_open_input(&impl_->format, impl_->path.c_str(), nullptr, nullptr) < 0) {
        sink.warn("decode.open", "Cannot open media for playback", impl_->path);
        return false;
    }
    if (avformat_find_stream_info(impl_->format, nullptr) < 0) {
        sink.warn("decode.streams", "No stream info", impl_->path);
        return false;
    }

    const AVCodec* decoder = nullptr;
    impl_->stream = av_find_best_stream(impl_->format, AVMEDIA_TYPE_VIDEO, -1, -1,
                                        &decoder, 0);
    if (impl_->stream < 0 || !decoder) {
        // Audio-only media is normal on an audio track; not worth a warning.
        MER_DEBUG("playback") << "no video stream in " << impl_->path;
        return false;
    }

    impl_->codec = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(impl_->codec,
                                  impl_->format->streams[impl_->stream]->codecpar);
    impl_->codec->thread_count = 1;

    if (avcodec_open2(impl_->codec, decoder, nullptr) < 0) {
        sink.warn("decode.codec", "Cannot open video decoder", impl_->path);
        avcodec_free_context(&impl_->codec);
        return false;
    }

    impl_->frame  = av_frame_alloc();
    impl_->packet = av_packet_alloc();

    MER_DEBUG("playback") << "opened " << impl_->path << " "
                          << impl_->codec->width << "x" << impl_->codec->height;
    return true;
}

bool VideoDecoder::frameAt(double seconds, int maxWidth, CachedFrame& out)
{
    if (!isOpen() || !impl_->ensureScaler(maxWidth)) {
        return false;
    }

    AVStream*    stream = impl_->format->streams[impl_->stream];
    const int64_t target =
        static_cast<int64_t>(seconds / av_q2d(stream->time_base));

    if (av_seek_frame(impl_->format, impl_->stream, target, AVSEEK_FLAG_BACKWARD) >= 0) {
        avcodec_flush_buffers(impl_->codec);
    }

    // Decode forward from the keyframe we landed on until we reach the frame
    // that actually covers `seconds`.
    bool decoded = false;
    int  guard   = 0;
    while (!decoded && guard++ < 240) {
        if (av_read_frame(impl_->format, impl_->packet) < 0) {
            break;
        }
        if (impl_->packet->stream_index != impl_->stream) {
            av_packet_unref(impl_->packet);
            continue;
        }
        if (avcodec_send_packet(impl_->codec, impl_->packet) == 0) {
            while (avcodec_receive_frame(impl_->codec, impl_->frame) == 0) {
                if (impl_->frame->best_effort_timestamp == AV_NOPTS_VALUE
                    || impl_->frame->best_effort_timestamp >= target) {
                    decoded = true;
                    break;
                }
            }
        }
        av_packet_unref(impl_->packet);
    }

    if (!decoded) {
        return false;
    }

    sws_scale(impl_->scaler, impl_->frame->data, impl_->frame->linesize, 0,
              impl_->codec->height, impl_->rgb->data, impl_->rgb->linesize);

    out.width  = impl_->scaledW;
    out.height = impl_->scaledH;
    out.pixels.resize(static_cast<std::size_t>(impl_->scaledW) * impl_->scaledH * 3);

    for (int y = 0; y < impl_->scaledH; ++y) {
        std::copy_n(impl_->rgb->data[0] + y * impl_->rgb->linesize[0],
                    impl_->scaledW * 3,
                    out.pixels.data() + static_cast<std::size_t>(y) * impl_->scaledW * 3);
    }
    return true;
}


} // namespace mer::playback
