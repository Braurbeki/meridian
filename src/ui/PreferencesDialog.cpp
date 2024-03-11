#include "ui/PreferencesDialog.h"

#include "ui/AppSettings.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QVBoxLayout>

namespace mer::ui {

PreferencesDialog::PreferencesDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    setMinimumWidth(420);

    auto* layout = new QVBoxLayout(this);

    auto* timelineGroup = new QGroupBox(tr("Timeline"), this);
    auto* timelineForm  = new QFormLayout(timelineGroup);

    trackHeight_ = new QSpinBox(timelineGroup);
    trackHeight_->setRange(24, 160);
    trackHeight_->setSuffix(tr(" px"));
    timelineForm->addRow(tr("Track height"), trackHeight_);

    maxLabelChars_ = new QSpinBox(timelineGroup);
    maxLabelChars_->setRange(8, 256);
    timelineForm->addRow(tr("Maximum caption length"), maxLabelChars_);

    truncateLabels_ = new QCheckBox(tr("Shorten long captions"), timelineGroup);
    timelineForm->addRow(QString(), truncateLabels_);

    showThumbnails_ = new QCheckBox(tr("Show thumbnails on segments"), timelineGroup);
    timelineForm->addRow(QString(), showThumbnails_);

    showSourceFilename_ =
        new QCheckBox(tr("Show source file name on segments"), timelineGroup);
    timelineForm->addRow(QString(), showSourceFilename_);

    layout->addWidget(timelineGroup);

    auto* mediaGroup = new QGroupBox(tr("Media"), this);
    auto* mediaForm  = new QFormLayout(mediaGroup);

    probeOnOpen_ = new QCheckBox(tr("Probe media when opening a project"), mediaGroup);
    mediaForm->addRow(QString(), probeOnOpen_);

    layout->addWidget(mediaGroup);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this] {
        save();
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    load();
}

void PreferencesDialog::load()
{
    AppSettings& s = AppSettings::instance();
    trackHeight_->setValue(s.trackHeight());
    maxLabelChars_->setValue(s.maxLabelChars());
    truncateLabels_->setChecked(s.truncateLabels());
    showThumbnails_->setChecked(s.showThumbnails());
    showSourceFilename_->setChecked(s.showSourceFilename());
    probeOnOpen_->setChecked(s.probeOnOpen());
}

void PreferencesDialog::save()
{
    AppSettings& s = AppSettings::instance();
    s.setTrackHeight(trackHeight_->value());
    s.setMaxLabelChars(maxLabelChars_->value());
    s.setTruncateLabels(truncateLabels_->isChecked());
    s.setShowThumbnails(showThumbnails_->isChecked());
    s.setShowSourceFilename(showSourceFilename_->isChecked());
    s.setProbeOnOpen(probeOnOpen_->isChecked());
}

} // namespace mer::ui
