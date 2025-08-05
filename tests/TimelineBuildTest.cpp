#include "TestMain.h"

#include "timeline/builder/TimelineBuilder.h"
#include "project/Project.h"
#include "util/Diagnostics.h"

using namespace mer;

namespace {

struct Fixture {
    project::Project     project;
    util::DiagnosticSink sink;

    /// Adds a clip backed by `fileName`, with an optional editorial name and
    /// an optional embedded title, and places it on the sequence.
    util::Uuid addSegment(const std::string& fileName, const std::string& editorialName,
                          const bool haveTitle, const std::string& title)
    {
        auto media = std::make_shared<core::MediaSource>("/media/" + fileName);
        media->setOnline(true);
        project.media().add(media);

        auto clip = std::make_shared<project::Clip>();
        clip->setMediaId(media->id());
        clip->setFrameCount(50);
        clip->setName(editorialName);
        if (haveTitle) {
            clip->metadata().setString("title", title);
        }
        project.rootBin().add(clip);

        if (project.timelines().empty()) {
            project::TimelineSpec spec;
            spec.name = "Sequence 1";
            spec.tracks.push_back(project::TrackSpec{});
            spec.tracks.front().name = "V1";
            project.timelines().push_back(std::move(spec));
        }

        project::SegmentSpec seg;
        seg.clipId     = clip->id();
        seg.frameCount = 50;
        seg.timelineStart =
            static_cast<std::int64_t>(
                project.timelines().front().tracks.front().segments.size())
            * 50;
        project.timelines().front().tracks.front().segments.push_back(seg);
        return clip->id();
    }

    timeline::TimelinePtr build()
    {
        const timeline::TimelineBuilder builder(project);
        return builder.build(project.timelines().front(), sink);
    }
};

std::string labelAt(const timeline::Timeline& tl, std::size_t index)
{
    return tl.tracks().front().segments()[index].label();
}

} // namespace

MER_TEST(editorialNameWinsOverEverything)
{
    Fixture f;
    f.addSegment("A001_C012.mxf", "Wide - take 4", true, "Scene 12");
    const auto built = f.build();
    MER_CHECK_EQ(labelAt(*built, 0), std::string("Wide - take 4"));
}

MER_TEST(embeddedTitleUsedWhenNoEditorialName)
{
    Fixture f;
    f.addSegment("A001_C013.mxf", "", true, "Scene 12");
    const auto built = f.build();
    MER_CHECK_EQ(labelAt(*built, 0), std::string("Scene 12"));
}

MER_TEST(fallsBackToFileName)
{
    Fixture f;
    f.addSegment("A001_C014.mxf", "", false, "");
    const auto built = f.build();
    MER_CHECK_EQ(labelAt(*built, 0), std::string("A001_C014.mxf"));
}

MER_TEST(buildsEverySegment)
{
    Fixture f;
    f.addSegment("A001_C012.mxf", "Wide - take 4", false, "");
    f.addSegment("A001_C013.mxf", "", true, "Scene 12");
    f.addSegment("A001_C014.mxf", "", false, "");

    const auto built = f.build();
    MER_CHECK_EQ(built->segmentCount(), std::size_t(3));
    MER_CHECK_EQ(built->tracks().size(), std::size_t(1));
    MER_CHECK_EQ(built->duration(), std::int64_t(150));
}

MER_TEST(missingClipIsReportedNotFatal)
{
    Fixture f;
    f.addSegment("A001_C012.mxf", "Wide", false, "");

    project::SegmentSpec dangling;
    dangling.clipId        = util::Uuid::generate();
    dangling.frameCount    = 25;
    dangling.timelineStart = 50;
    f.project.timelines().front().tracks.front().segments.push_back(dangling);

    const auto built = f.build();
    MER_CHECK_EQ(built->segmentCount(), std::size_t(2));
    MER_CHECK(f.sink.size() > 0);
}

MER_TEST(tooltipAlwaysNamesTheFile)
{
    Fixture f;
    f.addSegment("A001_C012.mxf", "Wide - take 4", true, "Scene 12");
    const auto built = f.build();
    const std::string tooltip = built->tracks().front().segments()[0].tooltip();
    MER_CHECK(tooltip.find("A001_C012.mxf") != std::string::npos);
}
