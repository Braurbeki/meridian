#pragma once
#include "core/TimeTypes.h"

#include <QWidget>

namespace mer::ui {

/// Timecode strip above the timeline.
class TimelineRuler : public QWidget {
    Q_OBJECT

public:
    explicit TimelineRuler(QWidget* parent = nullptr);

    void setRate(core::Rational rate);
    void setDuration(qint64 frames);
    void setPixelsPerFrame(double scale);
    void setPlayhead(qint64 frame);

    QSize sizeHint() const override { return QSize(800, 24); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int tickIntervalFrames() const;

    core::Rational rate_{25, 1};
    qint64         duration_       = 0;
    qint64         playhead_       = 0;
    double         pixelsPerFrame_ = 4.0;
};

} // namespace mer::ui
