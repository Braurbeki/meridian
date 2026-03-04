#pragma once
#include "playback/FrameSource.h"
#include "timeline/Timeline.h"

#include <QImage>
#include <QWidget>

namespace mer::ui {

/// Program monitor: the picture under the playhead, letterboxed, with a
/// slate overlay naming the clip and the timecode.
///
/// Decoding is pull-based and lazy -- the monitor asks the FrameSource for
/// the frame it needs when the playhead moves, and draws a slate when there
/// is nothing to show (offline media, an audio-only segment, a gap, or a
/// build without FFmpeg).
class MonitorView : public QWidget {
    Q_OBJECT

public:
    explicit MonitorView(QWidget* parent = nullptr);

    void setFrameSource(playback::FrameSource* source);
    void setTimeline(timeline::TimelinePtr timeline);
    void setPlayhead(qint64 frame);

    void setShowOverlay(bool on);
    bool showOverlay() const { return showOverlay_; }

    QSize sizeHint() const override { return QSize(880, 500); }
    QSize minimumSizeHint() const override { return QSize(320, 200); }

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void refreshFrame();
    QRect pictureRect() const;
    void  paintSlate(QPainter& painter, const QRect& target) const;
    void  paintOverlay(QPainter& painter, const QRect& target) const;

    const timeline::Segment* segmentUnderPlayhead() const;

    playback::FrameSource* source_ = nullptr;
    timeline::TimelinePtr  timeline_;
    qint64                 playhead_ = 0;

    QImage      image_;
    QString     caption_;
    QString     status_;
    bool        showOverlay_ = true;
    double      aspect_      = 16.0 / 9.0;
};

} // namespace mer::ui
