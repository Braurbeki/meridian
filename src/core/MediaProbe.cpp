#include "core/MediaProbe.h"

#include "util/Log.h"
#include "util/StringUtil.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/dict.h>
}

#include <cstdio>

namespace mer::core {
namespace {

StreamKind kindFromAv(int type)
{
    switch (type) {
    case AVMEDIA_TYPE_VIDEO:    return StreamKind::Video;
    case AVMEDIA_TYPE_AUDIO:    return StreamKind::Audio;
    case AVMEDIA_TYPE_SUBTITLE: return StreamKind::Subtitle;
    case AVMEDIA_TYPE_DATA:     return StreamKind::Data;
    default:                    return StreamKind::Unknown;
    }
}

/// Copies an AVDictionary verbatim, empty values included. Suppressing empty
/// tags here would hide real authoring problems from the ingest report.
void copyTags(const AVDictionary* src, MetadataDict& dst)
{
    const AVDictionaryEntry* entry = nullptr;
    while ((entry = av_dict_iterate(src, entry)) != nullptr) {
        dst.setString(entry->key, entry->value ? entry->value : "");
    }
}

} // namespace

bool MediaProbe::ffmpegAvailable()
{
    return true;
}

std::string MediaProbe::ffmpegVersionString()
{
    char buf[64];
    const unsigned v = avformat_version();
    std::snprintf(buf, sizeof(buf), "libavformat %u.%u.%u", v >> 16,
                  (v >> 8) & 0xff, v & 0xff);
    return std::string(buf);
}

bool MediaProbe::probe(MediaSource& source, util::DiagnosticSink& sink) const
{
    const std::string& target =
        source.resolvedPath().empty() ? source.path() : source.resolvedPath();

    if (target.empty()) {
        sink.warn("media.nopath", "Media source has no path", source.id().toString());
        source.setOnline(false);
        return false;
    }

    AVFormatContext* ctx = nullptr;
    const int        rc  = avformat_open_input(&ctx, target.c_str(), nullptr, nullptr);
    if (rc < 0) {
        char err[AV_ERROR_MAX_STRING_SIZE] = {0};
        av_strerror(rc, err, sizeof(err));
        sink.warn("media.offline",
                  "Cannot open media: " + std::string(err), target);
        source.setOnline(false);
        return false;
    }

    if (avformat_find_stream_info(ctx, nullptr) < 0) {
        sink.warn("media.nostreams", "No stream info available", target);
    }

    if (ctx->duration != AV_NOPTS_VALUE) {
        source.setDuration(static_cast<double>(ctx->duration) / AV_TIME_BASE);
    }

    copyTags(ctx->metadata, source.metadata());

    for (unsigned i = 0; i < ctx->nb_streams; ++i) {
        const AVStream* st = ctx->streams[i];
        StreamInfo      info;
        info.index     = static_cast<int>(i);
        info.kind      = kindFromAv(st->codecpar->codec_type);
        info.timeBase  = Rational{st->time_base.num, st->time_base.den};
        info.frameRate = Rational{st->avg_frame_rate.num, st->avg_frame_rate.den};

        if (const AVCodecDescriptor* desc = avcodec_descriptor_get(st->codecpar->codec_id)) {
            info.codec = desc->name;
        }

        if (info.kind == StreamKind::Video) {
            info.width  = st->codecpar->width;
            info.height = st->codecpar->height;
        } else if (info.kind == StreamKind::Audio) {
            info.channels   = st->codecpar->ch_layout.nb_channels;
            info.sampleRate = st->codecpar->sample_rate;
        }

        if (opts_.readStreamMetadata) {
            copyTags(st->metadata, info.metadata);
        }

        source.addStream(std::move(info));
    }

    avformat_close_input(&ctx);
    source.setOnline(true);

    MER_DEBUG("media") << "probed " << target << " streams=" << source.streams().size()
                       << " duration=" << source.duration();
    return true;
}

} // namespace mer::core
