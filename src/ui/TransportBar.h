#pragma once
#include "core/TimeTypes.h"

#include <QWidget>

class QLabel;
class QPushButton;
class QSlider;

namespace mer::ui {

class TransportBar : public QWidget {
    Q_OBJECT

public:
    explicit TransportBar(QWidget* parent = nullptr);

    void setRate(core::Rational rate);
    void setDuration(qint64 frames);
    void setPosition(qint64 frame);
    void setPlaying(bool playing);

signals:
    void playPauseRequested();
    void stopRequested();
    void stepRequested(int frames);
    void seekRequested(qint64 frame);

private:
    void updateTimecode();

    QPushButton*   playButton_ = nullptr;
    QPushButton*   stopButton_ = nullptr;
    QSlider*       scrubber_   = nullptr;
    QLabel*        timecode_   = nullptr;
    core::Rational rate_{25, 1};
    qint64         position_ = 0;
    qint64         duration_ = 0;
};

} // namespace mer::ui
