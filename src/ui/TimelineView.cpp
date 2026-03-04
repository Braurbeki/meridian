#include "ui/TimelineView.h"

#include "ui/AppSettings.h"
#include "ui/Theme.h"

#include <QFontMetrics>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>


namespace mer::ui {
namespace {

constexpr int kSegmentGap    = 1;
constexpr int kSegmentRadius = 3;

} // namespace

TimelineView::TimelineView(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(160);
    setMouseTracking(true);
    setAutoFillBackground(true);
    trackHeight_    = AppSettings::instance().trackHeight();
    pixelsPerFrame_ = AppSettings::instance().pixelsPerFrame();
}

void TimelineView::setTimeline(timeline::TimelinePtr timeline)
{
    timeline_ = std::move(timeline);
    playhead_ = 0;
    thumbnails_.clear();
    selectedId_.clear();
    updateGeometry();
    update();
}

void TimelineView::setFrameSource(playback::FrameSource* source)
{
    source_ = source;
    thumbnails_.clear();
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

void TimelineView::setSelectedSegment(const QString& segmentId)
{
    selectedId_ = segmentId;
    update();
}

void TimelineView::setPixelsPerFrame(double scale)
{
    pixelsPerFrame_ = qBound(0.25, scale, 64.0);
    AppSettings::instance().setPixelsPerFrame(pixelsPerFrame_);
    updateGeometry();
    update();
}

void TimelineView::zoomToFit()
{
    if (!timeline_) {
        return;
    }
    const qint64 duration = timeline_->duration();
    if (duration <= 0) {
        return;
    }

    // The scroll area's viewport is the real estate we actually have.
    const int available =
        (parentWidget() ? parentWidget()->width() : width()) - kHeadWidth - 24;
    if (available < 120) {
        return;
    }

    setPixelsPerFrame(static_cast<double>(available) / static_cast<double>(duration));
}

QSize TimelineView::sizeHint() const
{
    if (!timeline_) {
        return QSize(900, 200);
    }
    const int width = kHeadWidth
        + static_cast<int>(timeline_->duration() * pixelsPerFrame_) + 60;
    const int height =
        static_cast<int>(timeline_->tracks().size()) * trackHeight_ + 20;
    return QSize(qMax(width, 900), qMax(height, 160));
}

QRect TimelineView::trackRect(int index) const
{
    return QRect(0, 8 + index * trackHeight_, width(), trackHeight_ - 2);
}

QRect TimelineView::segmentRect(const timeline::Segment& segment,
                                const QRect& track) const
{
    const int x = kHeadWidth + static_cast<int>(segment.start() * pixelsPerFrame_);
    const int w = qMax(3, static_cast<int>(segment.frameCount() * pixelsPerFrame_)
                              - kSegmentGap);
    return QRect(x, track.top() + 3, w, track.height() - 6);
}

qint64 TimelineView::frameAtX(int x) const
{
    if (pixelsPerFrame_ <= 0.0) {
        return 0;
    }
    return qMax<qint64>(0, static_cast<qint64>((x - kHeadWidth) / pixelsPerFrame_));
}

QPixmap TimelineView::thumbnailFor(const timeline::Segment& segment, int height)
{
    const QString key = QString::fromStdString(segment.id().toString());
    const auto    hit = thumbnails_.constFind(key);
    if (hit != thumbnails_.constEnd()) {
        return hit.value();
    }

    QPixmap pixmap;  // a null pixmap is cached too, so we only try once
    if (source_ && segment.media() && segment.mediaOnline() && height > 0) {
        const core::MediaSourcePtr& media = segment.media();
        const std::string           path  = media->resolvedPath().empty()
                                                ? media->path()
                                                : media->resolvedPath();

        // Negative keys keep thumbnails from colliding with the monitor's
        // playhead-keyed entries in the shared frame cache.
        const auto hash = static_cast<std::int64_t>(qHash(key));
        const std::int64_t cacheKey = -(hash & 0x3fffffff) - 1;

        playback::CachedFrame frame;
        if (source_->frame(path, 0.15, height * 2, cacheKey, frame)) {
            const QImage image(frame.pixels.data(), frame.width, frame.height,
                               frame.width * 3, QImage::Format_RGB888);
            pixmap = QPixmap::fromImage(
                image.scaledToHeight(height, Qt::SmoothTransformation));
        }
    }

    thumbnails_.insert(key, pixmap);
    return pixmap;
}

void TimelineView::paintTrackHead(QPainter& painter, const timeline::Track& track,
                                  int index, const QRect& rect) const
{
    const QRect head(0, rect.top(), kHeadWidth, rect.height());
    painter.fillRect(head, theme::trackHead());

    painter.setPen(QPen(QColor(0x14, 0x16, 0x1a), 1));
    painter.drawLine(head.right(), head.top(), head.right(), head.bottom());

    const bool isAudio = track.kind() == timeline::TrackKind::Audio;

    // Kind badge.
    const QRect badge(head.left() + 10, head.center().y() - 9, 26, 18);
    QPainterPath badgePath;
    badgePath.addRoundedRect(badge, 3, 3);
    painter.fillPath(badgePath,
                     isAudio ? theme::audioSegmentTop() : theme::videoSegmentTop());

    QFont small = painter.font();
    small.setPointSizeF(small.pointSizeF() * 0.85);
    small.setBold(true);
    painter.setFont(small);
    painter.setPen(QColor(0xea, 0xee, 0xf4));
    painter.drawText(badge, Qt::AlignCenter, isAudio ? QStringLiteral("A") : QStringLiteral("V"));

    painter.setFont(QFont());
    painter.setPen(theme::text());
    painter.drawText(head.adjusted(44, 0, -34, 0), Qt::AlignVCenter | Qt::AlignLeft,
                     QString::fromStdString(track.header()));

    if (track.muted()) {
        painter.setPen(theme::warning());
        painter.drawText(head.adjusted(0, 0, -10, 0),
                         Qt::AlignVCenter | Qt::AlignRight, QStringLiteral("M"));
    } else if (track.locked()) {
        painter.setPen(theme::textDim());
        painter.drawText(head.adjusted(0, 0, -10, 0),
                         Qt::AlignVCenter | Qt::AlignRight, QStringLiteral("L"));
    }
}

void TimelineView::paintSegment(QPainter& painter, const timeline::Segment& segment,
                                const timeline::Track& track, const QRect& rect)
{
    const bool isAudio  = track.kind() == timeline::TrackKind::Audio;
    const bool offline  = !segment.mediaOnline();
    const bool selected = !selectedId_.isEmpty()
        && selectedId_ == QString::fromStdString(segment.id().toString());

    QColor top    = offline ? theme::offlineSegmentTop()
                            : (isAudio ? theme::audioSegmentTop() : theme::videoSegmentTop());
    QColor bottom = offline ? theme::offlineSegment()
                            : (isAudio ? theme::audioSegment() : theme::videoSegment());

    QPainterPath path;
    path.addRoundedRect(rect, kSegmentRadius, kSegmentRadius);
    painter.setClipPath(path);

    QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
    gradient.setColorAt(0.0, top);
    gradient.setColorAt(1.0, bottom);
    painter.fillRect(rect, gradient);

    // Head-frame thumbnail, the way a bin-linked segment usually reads.
    if (!isAudio && !offline && rect.width() > 56) {
        const QPixmap thumb = thumbnailFor(segment, rect.height());
        if (!thumb.isNull()) {
            const int drawWidth = qMin(thumb.width(), rect.width() - 2);
            painter.setOpacity(0.55);
            painter.drawPixmap(rect.left(), rect.top(), thumb, 0, 0, drawWidth,
                               thumb.height());
            painter.setOpacity(1.0);

            QLinearGradient fade(rect.left(), 0, rect.left() + drawWidth + 24, 0);
            fade.setColorAt(0.0, QColor(top.red(), top.green(), top.blue(), 40));
            fade.setColorAt(1.0, QColor(top.red(), top.green(), top.blue(), 235));
            painter.fillRect(QRect(rect.left(), rect.top(), drawWidth + 24,
                                   rect.height()),
                             fade);
        }
    }

    if (isAudio) {
        painter.setPen(QPen(QColor(0xff, 0xff, 0xff, 40), 1));
        painter.drawLine(rect.left() + 2, rect.center().y(), rect.right() - 2,
                         rect.center().y());
    }

    // Top highlight, the standard bevel.
    painter.setPen(QPen(QColor(0xff, 0xff, 0xff, 32), 1));
    painter.drawLine(rect.left() + 1, rect.top() + 1, rect.right() - 1, rect.top() + 1);

    painter.setClipping(false);

    painter.setPen(QPen(selected ? theme::accent() : QColor(0x10, 0x12, 0x15),
                        selected ? 2 : 1));
    painter.drawPath(path);

    const QRect textRect = rect.adjusted(9, 0, -8, -1);
    if (textRect.width() < 16) {
        return;
    }

    const QString      label = QString::fromStdString(segment.label());
    const QFontMetrics metrics(painter.font());
    const QString      elided =
        metrics.elidedText(label, Qt::ElideRight, textRect.width());

    painter.setPen(QColor(0, 0, 0, 130));
    painter.drawText(textRect.adjusted(1, 1, 1, 1), Qt::AlignVCenter | Qt::AlignLeft,
                     elided);
    painter.setPen(offline ? QColor(0xf0, 0xd0, 0xd0) : QColor(0xf2, 0xf4, 0xf7));
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, elided);

    if (offline && textRect.width() > 120) {
        painter.setPen(QColor(0xff, 0xc8, 0xc8));
        painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignRight,
                         QStringLiteral("OFFLINE"));
    }
}

void TimelineView::paintPlayhead(QPainter& painter) const
{
    const int x = kHeadWidth + static_cast<int>(playhead_ * pixelsPerFrame_);
    if (x < kHeadWidth) {
        return;
    }

    painter.setPen(QPen(theme::playhead(), 1));
    painter.drawLine(x, 0, x, height());

    QPainterPath head;
    head.moveTo(x - 5, 0);
    head.lineTo(x + 5, 0);
    head.lineTo(x, 9);
    head.closeSubpath();
    painter.fillPath(head, theme::playhead());
}

void TimelineView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), theme::window());

    if (!timeline_) {
        painter.setPen(theme::textFaint());
        painter.drawText(rect(), Qt::AlignCenter, tr("No sequence open"));
        return;
    }

    for (std::size_t i = 0; i < timeline_->tracks().size(); ++i) {
        const timeline::Track& track = timeline_->tracks()[i];
        const QRect            row   = trackRect(static_cast<int>(i));

        painter.fillRect(QRect(kHeadWidth, row.top(), width() - kHeadWidth,
                               row.height()),
                         (i % 2) ? theme::trackOdd() : theme::trackEven());

        for (const auto& segment : track.segments()) {
            paintSegment(painter, segment, track, segmentRect(segment, row));
        }
        paintTrackHead(painter, track, static_cast<int>(i), row);
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
            selectedId_ = QString::fromStdString(segment->id().toString());
            emit segmentClicked(selectedId_);
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
