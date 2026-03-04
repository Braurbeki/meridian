#include "ui/TimelineRuler.h"

#include "ui/Theme.h"
#include "ui/TimelineView.h"

#include <QPainter>

namespace mer::ui {

TimelineRuler::TimelineRuler(QWidget* parent) : QWidget(parent)
{
    setFixedHeight(26);
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
    painter.fillRect(rect(), theme::ruler());

    const int interval = tickIntervalFrames();
    const int offset   = TimelineView::kHeadWidth;

    painter.fillRect(QRect(0, 0, offset, height()), theme::trackHead());
    painter.setPen(QPen(QColor(0x14, 0x16, 0x1a), 1));
    painter.drawLine(offset, 0, offset, height());
    painter.drawLine(0, height() - 1, width(), height() - 1);

    QFont small = painter.font();
    small.setPointSizeF(small.pointSizeF() * 0.85);
    painter.setFont(small);

    for (qint64 frame = 0; frame <= duration_; frame += interval) {
        const int x = offset + static_cast<int>(frame * pixelsPerFrame_);
        if (x > width()) {
            break;
        }
        painter.setPen(theme::textFaint());
        painter.drawLine(x, height() - 7, x, height() - 1);

        // Half-way tick, unlabelled.
        const int mid = x + static_cast<int>(interval * pixelsPerFrame_ / 2.0);
        if (mid < width()) {
            painter.drawLine(mid, height() - 4, mid, height() - 1);
        }

        painter.setPen(theme::textDim());
        painter.drawText(x + 4, height() - 9,
                         QString::fromStdString(core::formatTimecode(frame, rate_)));
    }

    const int px = offset + static_cast<int>(playhead_ * pixelsPerFrame_);
    if (px >= offset) {
        painter.setPen(QPen(theme::playhead(), 1));
        painter.drawLine(px, 0, px, height());
    }
}

} // namespace mer::ui
