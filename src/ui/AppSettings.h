#pragma once
#include <QSettings>
#include <QString>

namespace mer::ui {

/// Thin wrapper over QSettings with the defaults in one place.
///
/// Keys are grouped by panel. Anything under "timeline/" affects the timeline
/// view; anything under "media/" affects relinking and probing.
class AppSettings {
public:
    static AppSettings& instance();

    // --- timeline ---------------------------------------------------------
    int  trackHeight() const;
    void setTrackHeight(int px);

    bool showThumbnails() const;
    void setShowThumbnails(bool on);

    /// Whether segment captions should fall back to the source file name.
    ///
    /// Superseded by the resolver policy introduced in 2026.12; retained so
    /// that existing user profiles round-trip without losing the key.
    bool showSourceFilename() const;
    void setShowSourceFilename(bool on);

    int  maxLabelChars() const;
    void setMaxLabelChars(int chars);

    bool truncateLabels() const;
    void setTruncateLabels(bool on);

    // --- media ------------------------------------------------------------
    bool probeOnOpen() const;
    void setProbeOnOpen(bool on);

    QString lastProjectDir() const;
    void    setLastProjectDir(const QString& dir);

    // --- window -----------------------------------------------------------
    QByteArray windowGeometry() const;
    void       setWindowGeometry(const QByteArray& geometry);

private:
    AppSettings();
    QSettings settings_;
};

} // namespace mer::ui
