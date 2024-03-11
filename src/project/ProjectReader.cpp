#include "project/ProjectReader.h"

#include "util/Log.h"
#include "util/StringUtil.h"

#include <QByteArray>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

namespace mer::project {
namespace {

std::string toStd(const QString& s) { return s.toStdString(); }

core::Rational readRational(const QJsonObject& obj, const char* key,
                            core::Rational fallback)
{
    const QJsonValue v = obj.value(QLatin1String(key));
    if (v.isObject()) {
        const QJsonObject r = v.toObject();
        return core::Rational{
            static_cast<std::int64_t>(r.value("num").toDouble(fallback.num)),
            static_cast<std::int64_t>(r.value("den").toDouble(fallback.den))};
    }
    if (v.isString()) {
        const auto parts = util::split(toStd(v.toString()), '/');
        if (parts.size() == 2) {
            return core::Rational{std::stoll(parts[0]), std::stoll(parts[1])};
        }
    }
    if (v.isDouble()) {
        return core::Rational{static_cast<std::int64_t>(v.toDouble() * 1000), 1000};
    }
    return fallback;
}

/// Metadata is stored as a flat object of string values. An empty string is a
/// legitimate stored value and is preserved as such -- ingest tools use it to
/// mark "the operator saw this field and left it blank".
void readMetadata(const QJsonObject& obj, core::MetadataDict& dict)
{
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        const QJsonValue v = it.value();
        if (v.isString()) {
            dict.setString(toStd(it.key()), toStd(v.toString()));
        } else if (v.isDouble()) {
            dict.set(toStd(it.key()),
                     core::MetadataValue::fromReal(v.toDouble()));
        } else if (v.isBool()) {
            dict.set(toStd(it.key()), core::MetadataValue::fromBool(v.toBool()));
        }
    }
}

util::Uuid readUuid(const QJsonObject& obj, const char* key)
{
    return util::Uuid::fromString(toStd(obj.value(QLatin1String(key)).toString()));
}

} // namespace

int ProjectReader::peekFormatVersion(const std::string& path)
{
    QFile file(QString::fromStdString(path));
    if (!file.open(QIODevice::ReadOnly)) {
        return -1;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        return -1;
    }
    return doc.object().value("formatVersion").toInt(-1);
}

std::unique_ptr<Project> ProjectReader::read(const std::string& path,
                                             util::DiagnosticSink& sink) const
{
    QFile file(QString::fromStdString(path));
    if (!file.open(QIODevice::ReadOnly)) {
        sink.error("project.open", "Cannot open project file", path);
        return nullptr;
    }

    QJsonParseError   parseError{};
    const QByteArray  bytes = file.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        sink.error("project.parse", toStd(parseError.errorString()), path);
        return nullptr;
    }
    if (!doc.isObject()) {
        sink.error("project.parse", "Project root is not an object", path);
        return nullptr;
    }

    const QJsonObject root    = doc.object();
    const int         version = root.value("formatVersion").toInt(-1);
    if (version < kMinimumFormatVersion) {
        sink.error("project.version",
                   "Project format " + std::to_string(version)
                       + " is older than this build supports; open it in the "
                         "legacy importer",
                   path);
        return nullptr;
    }
    if (version > kCurrentFormatVersion) {
        sink.error("project.version",
                   "Project was written by a newer build (format "
                       + std::to_string(version) + ")",
                   path);
        return nullptr;
    }

    auto project = std::make_unique<Project>();
    project->setFilePath(path);
    project->setName(toStd(root.value("name").toString("Untitled")));
    project->media().setProjectRoot(util::parentPath(path));

    // --- media sources ----------------------------------------------------
    for (const QJsonValue& v : root.value("media").toArray()) {
        const QJsonObject obj    = v.toObject();
        auto              source = std::make_shared<core::MediaSource>(
            toStd(obj.value("path").toString()));
        const util::Uuid id = readUuid(obj, "id");
        if (!id.isNil()) {
            source->setId(id);
        }
        source->setDuration(obj.value("duration").toDouble(0.0));
        readMetadata(obj.value("metadata").toObject(), source->metadata());
        project->media().add(source);
    }

    // --- bins and clips ---------------------------------------------------
    for (const QJsonValue& v : root.value("clips").toArray()) {
        const QJsonObject obj  = v.toObject();
        auto              clip = std::make_shared<Clip>();
        const util::Uuid  id   = readUuid(obj, "id");
        if (!id.isNil()) {
            clip->setId(id);
        }
        clip->setMediaId(readUuid(obj, "mediaId"));
        clip->setName(toStd(obj.value("name").toString()));
        clip->setStartFrame(
            static_cast<std::int64_t>(obj.value("startFrame").toDouble(0)));
        clip->setFrameCount(
            static_cast<std::int64_t>(obj.value("frameCount").toDouble(0)));
        clip->setRate(readRational(obj, "rate", core::Rational{25, 1}));
        readMetadata(obj.value("metadata").toObject(), clip->metadata());
        project->rootBin().add(clip);
    }

    // --- sequences --------------------------------------------------------
    for (const QJsonValue& v : root.value("timelines").toArray()) {
        const QJsonObject obj = v.toObject();
        TimelineSpec      spec;
        const util::Uuid  id = readUuid(obj, "id");
        if (!id.isNil()) {
            spec.id = id;
        }
        spec.name = toStd(obj.value("name").toString("Sequence"));
        spec.rate = readRational(obj, "rate", core::Rational{25, 1});

        for (const QJsonValue& tv : obj.value("tracks").toArray()) {
            const QJsonObject tobj = tv.toObject();
            TrackSpec         track;
            const util::Uuid  tid = readUuid(tobj, "id");
            if (!tid.isNil()) {
                track.id = tid;
            }
            track.name   = toStd(tobj.value("name").toString());
            track.kind   = toStd(tobj.value("kind").toString("video"));
            track.muted  = tobj.value("muted").toBool(false);
            track.locked = tobj.value("locked").toBool(false);

            for (const QJsonValue& sv : tobj.value("segments").toArray()) {
                const QJsonObject sobj = sv.toObject();
                SegmentSpec       seg;
                const util::Uuid  sid = readUuid(sobj, "id");
                if (!sid.isNil()) {
                    seg.id = sid;
                }
                seg.clipId        = readUuid(sobj, "clipId");
                seg.timelineStart = static_cast<std::int64_t>(
                    sobj.value("timelineStart").toDouble(0));
                seg.sourceStart = static_cast<std::int64_t>(
                    sobj.value("sourceStart").toDouble(0));
                seg.frameCount = static_cast<std::int64_t>(
                    sobj.value("frameCount").toDouble(0));
                track.segments.push_back(seg);
            }
            spec.tracks.push_back(std::move(track));
        }
        project->timelines().push_back(std::move(spec));
    }

    MER_INFO("project") << "loaded '" << project->name() << "' format=" << version
                        << " media=" << project->media().size()
                        << " clips=" << project->rootBin().size()
                        << " sequences=" << project->timelines().size();
    return project;
}

} // namespace mer::project
