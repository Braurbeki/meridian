#pragma once
#include <QDialog>

class QCheckBox;
class QSpinBox;

namespace mer::ui {

/// Edits the settings under AppSettings. Changes apply on accept and take
/// effect on the next sequence rebuild.
class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);

private:
    void load();
    void save();

    QSpinBox*  trackHeight_        = nullptr;
    QSpinBox*  maxLabelChars_      = nullptr;
    QCheckBox* truncateLabels_     = nullptr;
    QCheckBox* showThumbnails_     = nullptr;
    QCheckBox* showSourceFilename_ = nullptr;
    QCheckBox* probeOnOpen_        = nullptr;
};

} // namespace mer::ui
