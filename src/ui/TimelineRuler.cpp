#include "ui/TimelineRuler.h"

#include <QPainter>

namespace mer::ui {

TimelineRuler::TimelineRuler(QWidget* parent) : QWidget(parent)
{
    setFixedHeight(24);
}

void TimelineRuler::setRate(core::Rational rate)
{
    rate_ = rate;
    update();
}

void TimelineRuler::setDuration(qint64 frames)
{
    duration_ = frames;
    update();
}

void TimelineRuler::setPixelsPerFrame(double scale)
{
    pixelsPerFrame_ = scale;
    update();
}

void TimelineRuler::setPlayhead(qint64 frame)
{
    playhead_ = frame;
    update();
}

int TimelineRuler::tickIntervalFrames() const
{
    const int fps = qMax(1, static_cast<int>(rate_.toDouble() + 0.5));
    for (const int seconds : {1, 2, 5, 10, 30, 60, 300}) {
        if (seconds * fps * pixelsPerFrame_ >= 72.0) {
            return seconds * fps;
        }
    }
    return 600 * fps;
}

void TimelineRuler::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0x1c, 0x1e, 0x22));
    painter.setPen(QColor(0x9a, 0x9e, 0xa6));

    const int interval = tickIntervalFrames();
    const int offset   = 96;  // matches TimelineView::kHeadWidth

    for (qint64 frame = 0; frame <= duration_; frame += interval) {
        const int x = offset + static_cast<int>(frame * pixelsPerFrame_);
        if (x > width()) {
            break;
        }
        painter.drawLine(x, height() - 6, x, height());
        painter.drawText(x + 3, height() - 8,
                         QString::fromStdString(core::formatTimecode(frame, rate_)));
    }

    const int px = offset + static_cast<int>(playhead_ * pixelsPerFrame_);
    painter.setPen(QPen(QColor(0xf2, 0x6d, 0x5b), 2));
    painter.drawLine(px, 0, px, height());
}

} // namespace mer::ui
