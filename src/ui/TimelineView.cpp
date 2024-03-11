#include "ui/TimelineView.h"

#include "ui/AppSettings.h"

#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

namespace mer::ui {
namespace {

const QColor kBackground(0x22, 0x24, 0x28);
const QColor kTrackBackground(0x2b, 0x2e, 0x34);
const QColor kHeadBackground(0x33, 0x36, 0x3d);
const QColor kSegmentFill(0x3d, 0x6b, 0x9e);
const QColor kSegmentOffline(0x6b, 0x4a, 0x4a);
const QColor kSegmentBorder(0x1a, 0x1c, 0x20);
const QColor kLabelColor(0xea, 0xec, 0xf0);
const QColor kPlayheadColor(0xf2, 0x6d, 0x5b);

} // namespace

TimelineView::TimelineView(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(160);
    setMouseTracking(true);
    setAutoFillBackground(true);
    trackHeight_ = AppSettings::instance().trackHeight();
}

void TimelineView::setTimeline(timeline::TimelinePtr timeline)
{
    timeline_ = std::move(timeline);
    playhead_ = 0;
    updateGeometry();
    update();
}

void TimelineView::setPlayhead(qint64 frame)
{
    if (playhead_ == frame) {
        return;
    }
    playhead_ = frame;
    update();
}

void TimelineView::setPixelsPerFrame(double scale)
{
    pixelsPerFrame_ = qBound(0.25, scale, 64.0);
    updateGeometry();
    update();
}

QSize TimelineView::sizeHint() const
{
    if (!timeline_) {
        return QSize(800, 160);
    }
    const int width = kHeadWidth
        + static_cast<int>(timeline_->duration() * pixelsPerFrame_) + 40;
    const int height = static_cast<int>(timeline_->tracks().size()) * trackHeight_ + 16;
    return QSize(qMax(width, 800), qMax(height, 160));
}

QRect TimelineView::trackRect(int index) const
{
    return QRect(0, 8 + index * trackHeight_, width(), trackHeight_ - 4);
}

QRect TimelineView::segmentRect(const timeline::Segment& segment,
                                const QRect& track) const
{
    const int x = kHeadWidth + static_cast<int>(segment.start() * pixelsPerFrame_);
    const int w = qMax(2, static_cast<int>(segment.frameCount() * pixelsPerFrame_));
    return QRect(x, track.top() + 4, w, track.height() - 8);
}

qint64 TimelineView::frameAtX(int x) const
{
    if (pixelsPerFrame_ <= 0.0) {
        return 0;
    }
    return qMax<qint64>(0, static_cast<qint64>((x - kHeadWidth) / pixelsPerFrame_));
}

void TimelineView::paintTrackHead(QPainter& painter, const timeline::Track& track,
                                  const QRect& rect) const
{
    const QRect head(0, rect.top(), kHeadWidth, rect.height());
    painter.fillRect(head, kHeadBackground);

    painter.setPen(kLabelColor);
    painter.drawText(head.adjusted(10, 0, -6, 0),
                     Qt::AlignVCenter | Qt::AlignLeft,
                     QString::fromStdString(track.header()));

    if (track.muted()) {
        painter.setPen(QColor(0xd0, 0x90, 0x60));
        painter.drawText(head.adjusted(0, 0, -8, 0),
                         Qt::AlignVCenter | Qt::AlignRight, QStringLiteral("M"));
    }
}

void TimelineView::paintSegment(QPainter& painter, const timeline::Segment& segment,
                                const QRect& rect) const
{
    painter.fillRect(rect, segment.mediaOnline() ? kSegmentFill : kSegmentOffline);
    painter.setPen(kSegmentBorder);
    painter.drawRect(rect.adjusted(0, 0, -1, -1));

    // TODO(2019-11): caption looks clipped a pixel early on HiDPI screens.
    // Suspect the elide width should account for the device pixel ratio.
    const QRect textRect = rect.adjusted(6, 0, -6, 0);
    if (textRect.width() < 12) {
        return;
    }

    const QString     label = QString::fromStdString(segment.label());
    const QFontMetrics metrics(painter.font());
    const QString      elided =
        metrics.elidedText(label, Qt::ElideMiddle, textRect.width());

    painter.setPen(kLabelColor);
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);
}

void TimelineView::paintPlayhead(QPainter& painter) const
{
    const int x = kHeadWidth + static_cast<int>(playhead_ * pixelsPerFrame_);
    painter.setPen(QPen(kPlayheadColor, 2));
    painter.drawLine(x, 0, x, height());
}

void TimelineView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), kBackground);

    if (!timeline_) {
        painter.setPen(QColor(0x80, 0x84, 0x8c));
        painter.drawText(rect(), Qt::AlignCenter,
                         QStringLiteral("No sequence open"));
        return;
    }

    for (std::size_t i = 0; i < timeline_->tracks().size(); ++i) {
        const timeline::Track& track = timeline_->tracks()[i];
        const QRect            rect  = trackRect(static_cast<int>(i));

        painter.fillRect(QRect(kHeadWidth, rect.top(), width() - kHeadWidth,
                               rect.height()),
                         kTrackBackground);

        for (const auto& segment : track.segments()) {
            paintSegment(painter, segment, segmentRect(segment, rect));
        }
        paintTrackHead(painter, track, rect);
    }

    paintPlayhead(painter);
}

void TimelineView::mousePressEvent(QMouseEvent* event)
{
    if (event->position().x() < kHeadWidth) {
        return;
    }
    const qint64 frame = frameAtX(static_cast<int>(event->position().x()));
    setPlayhead(frame);
    emit playheadMoved(frame);

    if (!timeline_) {
        return;
    }
    const int index = (static_cast<int>(event->position().y()) - 8) / trackHeight_;
    if (index >= 0 && static_cast<std::size_t>(index) < timeline_->tracks().size()) {
        if (const auto* segment = timeline_->tracks()[index].segmentAt(frame)) {
            emit segmentClicked(QString::fromStdString(segment->id().toString()));
        }
    }
}

void TimelineView::mouseMoveEvent(QMouseEvent* event)
{
    if (!timeline_ || event->position().x() < kHeadWidth) {
        setToolTip(QString());
        return;
    }
    const int index = (static_cast<int>(event->position().y()) - 8) / trackHeight_;
    if (index < 0 || static_cast<std::size_t>(index) >= timeline_->tracks().size()) {
        setToolTip(QString());
        return;
    }
    const qint64 frame = frameAtX(static_cast<int>(event->position().x()));
    if (const auto* segment = timeline_->tracks()[index].segmentAt(frame)) {
        setToolTip(QString::fromStdString(segment->tooltip()));
    } else {
        setToolTip(QString());
    }
}

void TimelineView::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        const double factor = event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
        setPixelsPerFrame(pixelsPerFrame_ * factor);
        event->accept();
        return;
    }
    QWidget::wheelEvent(event);
}

} // namespace mer::ui
