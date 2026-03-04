#include "ui/TransportBar.h"

#include "ui/Theme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

namespace mer::ui {

TransportBar::TransportBar(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 6, 10, 6);
    layout->setSpacing(6);
    setAutoFillBackground(true);

    auto* back = new QPushButton(QStringLiteral("<"), this);
    back->setFixedWidth(32);
    back->setToolTip(tr("Step back one frame"));
    layout->addWidget(back);

    playButton_ = new QPushButton(tr("Play"), this);
    playButton_->setFixedWidth(72);
    layout->addWidget(playButton_);

    auto* forward = new QPushButton(QStringLiteral(">"), this);
    forward->setFixedWidth(32);
    forward->setToolTip(tr("Step forward one frame"));
    layout->addWidget(forward);

    stopButton_ = new QPushButton(tr("Stop"), this);
    stopButton_->setFixedWidth(64);
    layout->addWidget(stopButton_);

    scrubber_ = new QSlider(Qt::Horizontal, this);
    scrubber_->setRange(0, 0);
    layout->addWidget(scrubber_, 1);

    timecode_ = new QLabel(this);
    timecode_->setMinimumWidth(190);
    timecode_->setAlignment(Qt::AlignCenter);
    timecode_->setStyleSheet(
        "font-family: Menlo, Consolas, 'DejaVu Sans Mono', monospace;"
        "font-size: 12px; color: #d8dbe0; background: #14161a;"
        "border: 1px solid #33383f; border-radius: 4px; padding: 4px 8px;");
    layout->addWidget(timecode_);

    connect(playButton_, &QPushButton::clicked, this,
            &TransportBar::playPauseRequested);
    connect(stopButton_, &QPushButton::clicked, this, &TransportBar::stopRequested);
    connect(back, &QPushButton::clicked, this, [this] { emit stepRequested(-1); });
    connect(forward, &QPushButton::clicked, this, [this] { emit stepRequested(1); });
    connect(scrubber_, &QSlider::sliderMoved, this,
            [this](int value) { emit seekRequested(value); });

    updateTimecode();
}

void TransportBar::setRate(core::Rational rate)
{
    rate_ = rate;
    updateTimecode();
}

void TransportBar::setDuration(qint64 frames)
{
    duration_ = frames;
    scrubber_->setRange(0, static_cast<int>(qMax<qint64>(frames - 1, 0)));
    updateTimecode();
}

void TransportBar::setPosition(qint64 frame)
{
    position_ = frame;
    if (!scrubber_->isSliderDown()) {
        scrubber_->setValue(static_cast<int>(frame));
    }
    updateTimecode();
}

void TransportBar::setPlaying(bool playing)
{
    playButton_->setText(playing ? tr("Pause") : tr("Play"));
}

void TransportBar::updateTimecode()
{
    timecode_->setText(QStringLiteral("%1 / %2")
                           .arg(QString::fromStdString(
                                    core::formatTimecode(position_, rate_)),
                                QString::fromStdString(
                                    core::formatTimecode(duration_, rate_))));
}

} // namespace mer::ui
