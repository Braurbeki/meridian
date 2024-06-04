#include "compat/v1/LegacyProjectImporter.h"

#include "util/Log.h"
#include "util/StringUtil.h"

#include <QFile>
#include <QString>
#include <QTextStream>

namespace mer::compat::v1 {
namespace {

constexpr const char* kMagic = "MRPX";

} // namespace

bool LegacyProjectImporter::canImport(const std::string& path)
{
#ifdef MERIDIAN_LEGACY_PROJECTS
    return util::toLower(util::extension(path)) == "mrpx";
#else
    (void)path;
    return false;
#endif
}

bool LegacyProjectImporter::parseHeader(const std::string& path, int& versionOut) const
{
    QFile file(QString::fromStdString(path));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream  stream(&file);
    const QString header = stream.readLine();
    if (!header.startsWith(QLatin1String(kMagic))) {
        return false;
    }
    versionOut = header.mid(4).trimmed().toInt();
    return versionOut >= 1 && versionOut <= 3;
}

std::unique_ptr<project::Project> LegacyProjectImporter::import(
    const std::string& path, util::DiagnosticSink& sink) const
{
#ifndef MERIDIAN_LEGACY_PROJECTS
    sink.error("compat.disabled",
               "This build was configured without legacy project support", path);
    return nullptr;
#else
    int version = 0;
    if (!parseHeader(path, version)) {
        sink.error("compat.header", "Not a recognisable v1 document", path);
        return nullptr;
    }

    MER_INFO("compat") << "importing legacy document v" << version << " from " << path;

    QFile file(QString::fromStdString(path));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        sink.error("compat.open", "Cannot read legacy document", path);
        return nullptr;
    }

    auto project = std::make_unique<project::Project>();
    project->setFilePath(path);
    project->media().setProjectRoot(util::parentPath(path));

    QTextStream stream(&file);
    stream.readLine();  // header, already validated

    project::TimelineSpec sequence;
    sequence.name = "Sequence 1";
    project::TrackSpec track;
    track.name = "V1";

    std::int64_t cursor = 0;
    while (!stream.atEnd()) {
        const std::string line = util::trim(stream.readLine().toStdString());
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // v1 rows are: CLIP<TAB>path<TAB>frames<TAB>name
        const auto fields = util::split(line, '\t');
        if (fields.size() < 3 || fields[0] != "CLIP") {
            sink.warn("compat.row", "Skipping unrecognised row", line);
            continue;
        }

        auto media = std::make_shared<core::MediaSource>(fields[1]);
        project->media().add(media);

        auto clip = std::make_shared<project::Clip>();
        clip->setMediaId(media->id());
        clip->setFrameCount(std::stoll(fields[2]));
        if (fields.size() > 3) {
            clip->setName(fields[3]);
        }
        project->rootBin().add(clip);

        project::SegmentSpec seg;
        seg.clipId        = clip->id();
        seg.timelineStart = cursor;
        seg.frameCount    = clip->frameCount();
        track.segments.push_back(seg);
        cursor += clip->frameCount();
    }

    sequence.tracks.push_back(std::move(track));
    project->timelines().push_back(std::move(sequence));

    sink.info("compat.imported",
              "Imported legacy document; segment captions keep the v1 rule "
              "until re-labelled",
              path);
    return project;
#endif
}

} // namespace mer::compat::v1
