#include "project/ProjectWriter.h"

#include "project/ProjectReader.h"
#include "util/Log.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace mer::project {
namespace {

QString qs(const std::string& s) { return QString::fromStdString(s); }

QJsonObject writeMetadata(const core::MetadataDict& dict)
{
    QJsonObject obj;
    for (const auto& kv : dict) {
        obj.insert(qs(kv.first), qs(kv.second.asString()));
    }
    return obj;
}

QJsonObject writeRational(core::Rational r)
{
    QJsonObject obj;
    obj.insert("num", static_cast<double>(r.num));
    obj.insert("den", static_cast<double>(r.den));
    return obj;
}

} // namespace

bool ProjectWriter::write(const Project& project, const std::string& path,
                          util::DiagnosticSink& sink) const
{
    QJsonObject root;
    root.insert("formatVersion", ProjectReader::kCurrentFormatVersion);
    root.insert("name", qs(project.name()));

    QJsonArray media;
    for (const auto& source : project.media().all()) {
        QJsonObject obj;
        obj.insert("id", qs(source->id().toString()));
        obj.insert("path", qs(source->path()));
        obj.insert("duration", source->duration());
        obj.insert("metadata", writeMetadata(source->metadata()));
        media.append(obj);
    }
    root.insert("media", media);

    QJsonArray clips;
    for (const auto& clip : project.rootBin().clips()) {
        QJsonObject obj;
        obj.insert("id", qs(clip->id().toString()));
        obj.insert("mediaId", qs(clip->mediaId().toString()));
        obj.insert("name", qs(clip->name()));
        obj.insert("startFrame", static_cast<double>(clip->startFrame()));
        obj.insert("frameCount", static_cast<double>(clip->frameCount()));
        obj.insert("rate", writeRational(clip->rate()));
        obj.insert("metadata", writeMetadata(clip->metadata()));
        clips.append(obj);
    }
    root.insert("clips", clips);

    QJsonArray timelines;
    for (const auto& spec : project.timelines()) {
        QJsonObject tl;
        tl.insert("id", qs(spec.id.toString()));
        tl.insert("name", qs(spec.name));
        tl.insert("rate", writeRational(spec.rate));

        QJsonArray tracks;
        for (const auto& track : spec.tracks) {
            QJsonObject tobj;
            tobj.insert("id", qs(track.id.toString()));
            tobj.insert("name", qs(track.name));
            tobj.insert("kind", qs(track.kind));
            tobj.insert("muted", track.muted);
            tobj.insert("locked", track.locked);

            QJsonArray segments;
            for (const auto& seg : track.segments) {
                QJsonObject sobj;
                sobj.insert("id", qs(seg.id.toString()));
                sobj.insert("clipId", qs(seg.clipId.toString()));
                sobj.insert("timelineStart", static_cast<double>(seg.timelineStart));
                sobj.insert("sourceStart", static_cast<double>(seg.sourceStart));
                sobj.insert("frameCount", static_cast<double>(seg.frameCount));
                segments.append(sobj);
            }
            tobj.insert("segments", segments);
            tracks.append(tobj);
        }
        tl.insert("tracks", tracks);
        timelines.append(tl);
    }
    root.insert("timelines", timelines);

    const QString tempPath = qs(path) + ".tmp";
    QFile         file(tempPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        sink.error("project.write", "Cannot open project for writing", path);
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();

    QFile::remove(qs(path));
    if (!QFile::rename(tempPath, qs(path))) {
        sink.error("project.write", "Cannot replace project file", path);
        return false;
    }

    MER_INFO("project") << "saved '" << project.name() << "' to " << path;
    return true;
}

} // namespace mer::project
