#include "ui/AppSettings.h"

namespace mer::ui {

AppSettings::AppSettings() : settings_("Meridian", "Meridian") {}

AppSettings& AppSettings::instance()
{
    static AppSettings s;
    return s;
}

int AppSettings::trackHeight() const
{
    return settings_.value("timeline/trackHeight", 56).toInt();
}

void AppSettings::setTrackHeight(int px)
{
    settings_.setValue("timeline/trackHeight", px);
}

bool AppSettings::showThumbnails() const
{
    return settings_.value("timeline/showThumbnails", false).toBool();
}

void AppSettings::setShowThumbnails(bool on)
{
    settings_.setValue("timeline/showThumbnails", on);
}

bool AppSettings::showSourceFilename() const
{
    return settings_.value("timeline/showSourceFilename", true).toBool();
}

void AppSettings::setShowSourceFilename(bool on)
{
    settings_.setValue("timeline/showSourceFilename", on);
}

double AppSettings::pixelsPerFrame() const
{
    return settings_.value("timeline/pixelsPerFrame", 4.0).toDouble();
}

void AppSettings::setPixelsPerFrame(double scale)
{
    settings_.setValue("timeline/pixelsPerFrame", scale);
}

int AppSettings::maxLabelChars() const
{
    return settings_.value("timeline/maxLabelChars", 64).toInt();
}

void AppSettings::setMaxLabelChars(int chars)
{
    settings_.setValue("timeline/maxLabelChars", chars);
}

bool AppSettings::truncateLabels() const
{
    return settings_.value("timeline/truncateLabels", true).toBool();
}

void AppSettings::setTruncateLabels(bool on)
{
    settings_.setValue("timeline/truncateLabels", on);
}

bool AppSettings::probeOnOpen() const
{
    return settings_.value("media/probeOnOpen", true).toBool();
}

void AppSettings::setProbeOnOpen(bool on)
{
    settings_.setValue("media/probeOnOpen", on);
}

QString AppSettings::lastProjectDir() const
{
    return settings_.value("media/lastProjectDir").toString();
}

void AppSettings::setLastProjectDir(const QString& dir)
{
    settings_.setValue("media/lastProjectDir", dir);
}

QByteArray AppSettings::windowGeometry() const
{
    return settings_.value("window/geometry").toByteArray();
}

void AppSettings::setWindowGeometry(const QByteArray& geometry)
{
    settings_.setValue("window/geometry", geometry);
}

} // namespace mer::ui
