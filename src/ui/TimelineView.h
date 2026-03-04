#pragma once
#include "playback/FrameSource.h"
#include "timeline/Timeline.h"

#include <QHash>
#include <QPixmap>
#include <QWidget>

namespace mer::ui {

/// Draws a built timeline: track heads on the left, segment rectangles to the
/// right, playhead on top.
///
/// The view is a pure renderer. Every string it draws was computed by
/// TimelineBuilder -- if a caption looks wrong, the cause is upstream of this
/// file.
class TimelineView : public QWidget {
    Q_OBJECT

public:
    explicit TimelineView(QWidget* parent = nullptr);

    void setTimeline(timeline::TimelinePtr timeline);
    const timeline::TimelinePtr& timeline() const { return timeline_; }

    /// Optional: supplies head-frame thumbnails for video segments.
    void setFrameSource(playback::FrameSource* source);

    void setPlayhead(qint64 frame);
    qint64 playhead() const { return playhead_; }

    void   setPixelsPerFrame(double scale);
    double pixelsPerFrame() const { return pixelsPerFrame_; }

    void setSelectedSegment(const QString& segmentId);

    /// Picks a zoom that shows the whole sequence in the space available.
    void zoomToFit();

    static constexpr int kHeadWidth = 132;

    QSize sizeHint() const override;

signals:
    void playheadMoved(qint64 frame);
    void segmentClicked(const QString& segmentId);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void paintTrackHead(QPainter& painter, const timeline::Track& track, int index,
                        const QRect& rect) const;
    void paintSegment(QPainter& painter, const timeline::Segment& segment,
                      const timeline::Track& track, const QRect& rect);
    void paintPlayhead(QPainter& painter) const;

    QPixmap thumbnailFor(const timeline::Segment& segment, int height);

    QRect  trackRect(int index) const;
    QRect  segmentRect(const timeline::Segment& segment, const QRect& track) const;
    qint64 frameAtX(int x) const;

    timeline::TimelinePtr  timeline_;
    playback::FrameSource* source_ = nullptr;

    qint64 playhead_       = 0;
    double pixelsPerFrame_ = 4.0;
    int    trackHeight_    = 64;
    QString selectedId_;

    QHash<QString, QPixmap> thumbnails_;
};

} // namespace mer::ui
