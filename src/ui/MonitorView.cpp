#include "ui/MonitorView.h"

#include "ui/Theme.h"

#include <QPainter>
#include <QResizeEvent>

namespace mer::ui {

MonitorView::MonitorView(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(220);
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor(0x0b, 0x0c, 0x0e));
    setPalette(p);
}

void MonitorView::setFrameSource(playback::FrameSource* source)
{
    source_ = source;
    refreshFrame();
    update();
}

void MonitorView::setTimeline(timeline::TimelinePtr timeline)
{
    timeline_ = std::move(timeline);
    playhead_ = 0;
    refreshFrame();
    update();
}

void MonitorView::setPlayhead(qint64 frame)
{
    if (playhead_ == frame) {
        return;
    }
    playhead_ = frame;
    refreshFrame();
    update();
}

void MonitorView::setShowOverlay(bool on)
{
    showOverlay_ = on;
    update();
}

void MonitorView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    refreshFrame();
}

const timeline::Segment* MonitorView::segmentUnderPlayhead() const
{
    if (!timeline_) {
        return nullptr;
    }
    for (const auto& track : timeline_->tracks()) {
        if (track.kind() != timeline::TrackKind::Video) {
            continue;
        }
        if (const auto* segment = track.segmentAt(playhead_)) {
            return segment;
        }
    }
    return nullptr;
}

void MonitorView::refreshFrame()
{
    image_   = QImage();
    caption_.clear();
    status_.clear();

    const timeline::Segment* segment = segmentUnderPlayhead();
    if (!segment) {
        status_ = timeline_ ? tr("No picture at this position") : tr("No sequence open");
        return;
    }

    caption_ = QString::fromStdString(segment->label());

    if (!segment->media()) {
        status_ = tr("Media not in library");
        return;
    }
    if (!segment->mediaOnline()) {
        status_ = tr("Media offline");
        return;
    }
    if (!source_) {
        status_ = tr("No decoder");
        return;
    }

    const core::MediaSourcePtr& media = segment->media();
    const std::string           path =
        media->resolvedPath().empty() ? media->path() : media->resolvedPath();

    const double rate = timeline_->rate().toDouble();
    if (rate <= 0.0) {
        status_ = tr("Unknown frame rate");
        return;
    }

    // Position within the source file, not the sequence.
    const qint64 intoSegment = playhead_ - segment->start();
    const double seconds     = static_cast<double>(intoSegment) / rate;

    const int targetWidth = qMax(160, pictureRect().width());

    playback::CachedFrame frame;
    if (!source_->frame(path, seconds, targetWidth, playhead_, frame)) {
        status_ = tr("No video stream");
        return;
    }

    image_ = QImage(frame.pixels.data(), frame.width, frame.height,
                    frame.width * 3, QImage::Format_RGB888)
                 .copy();
    if (frame.height > 0) {
        aspect_ = static_cast<double>(frame.width) / frame.height;
    }
}

QRect MonitorView::pictureRect() const
{
    const QRect box = rect().adjusted(12, 12, -12, -12);
    if (box.width() <= 0 || box.height() <= 0) {
        return box;
    }

    int w = box.width();
    int h = static_cast<int>(w / aspect_);
    if (h > box.height()) {
        h = box.height();
        w = static_cast<int>(h * aspect_);
    }
    return QRect(box.x() + (box.width() - w) / 2, box.y() + (box.height() - h) / 2,
                 w, h);
}

void MonitorView::paintSlate(QPainter& painter, const QRect& target) const
{
    painter.fillRect(target, QColor(0x12, 0x14, 0x17));

    painter.setPen(QPen(QColor(0x2a, 0x2e, 0x35), 1));
    painter.drawRect(target.adjusted(0, 0, -1, -1));

    // Framing cross, the way a blanked monitor usually looks.
    painter.setPen(QPen(QColor(0x1e, 0x22, 0x28), 1));
    painter.drawLine(target.left(), target.center().y(), target.right(),
                     target.center().y());
    painter.drawLine(target.center().x(), target.top(), target.center().x(),
                     target.bottom());

    painter.setPen(theme::textFaint());
    QFont f = painter.font();
    f.setPointSizeF(f.pointSizeF() * 1.1);
    painter.setFont(f);
    painter.drawText(target, Qt::AlignCenter, status_);
}

void MonitorView::paintOverlay(QPainter& painter, const QRect& target) const
{
    const int barHeight = 30;
    const QRect bar(target.left(), target.bottom() - barHeight + 1, target.width(),
                    barHeight);

    painter.fillRect(bar, QColor(0, 0, 0, 150));

    QFont f = painter.font();
    f.setPointSizeF(f.pointSizeF() * 0.95);
    painter.setFont(f);

    painter.setPen(QColor(0xe8, 0xea, 0xee));
    painter.drawText(bar.adjusted(10, 0, -10, 0), Qt::AlignVCenter | Qt::AlignLeft,
                     caption_.isEmpty() ? tr("(no name)") : caption_);

    QFont mono = f;
    // setFamily() takes one family, not a fallback list -- Qt would look for a
    // face literally named "Menlo, Consolas, ..." and fall back to the default.
    mono.setFamilies({QStringLiteral("Menlo"), QStringLiteral("Consolas"),
                      QStringLiteral("DejaVu Sans Mono"), QStringLiteral("monospace")});
    painter.setFont(mono);
    painter.setPen(QColor(0xb6, 0xbc, 0xc6));

    const QString tc = timeline_
        ? QString::fromStdString(core::formatTimecode(playhead_, timeline_->rate()))
        : QStringLiteral("--:--:--:--");
    painter.drawText(bar.adjusted(10, 0, -10, 0), Qt::AlignVCenter | Qt::AlignRight, tc);
}

void MonitorView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0x0b, 0x0c, 0x0e));

    const QRect target = pictureRect();
    if (target.width() <= 0 || target.height() <= 0) {
        return;
    }

    if (image_.isNull()) {
        paintSlate(painter, target);
    } else {
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.drawImage(target, image_);
        painter.setPen(QPen(QColor(0x00, 0x00, 0x00), 1));
        painter.drawRect(target.adjusted(0, 0, -1, -1));
    }

    if (showOverlay_) {
        paintOverlay(painter, target);
    }
}

} // namespace mer::ui
