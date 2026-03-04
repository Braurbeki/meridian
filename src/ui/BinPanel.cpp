#include "ui/BinPanel.h"

#include "util/StringUtil.h"

#include <QHeaderView>

namespace mer::ui {

BinPanel::BinPanel(QWidget* parent) : QTreeWidget(parent)
{
    setColumnCount(4);
    setHeaderLabels({tr("Name"), tr("File"), tr("Frames"), tr("Status")});
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    setMinimumWidth(380);

    connect(this, &QTreeWidget::itemSelectionChanged, this, [this] {
        const auto items = selectedItems();
        if (!items.isEmpty()) {
            emit clipSelected(items.front()->data(0, Qt::UserRole).toString());
        }
    });
}

void BinPanel::setProject(const project::Project* project)
{
    project_ = project;
    refresh();
}

void BinPanel::addClipRow(const project::Clip& clip)
{
    const core::MediaSourcePtr media = project_->media().find(clip.mediaId());

    QString displayName = QString::fromStdString(clip.name());
    if (displayName.isEmpty() && media) {
        displayName = QString::fromStdString(util::stemName(media->fileName()));
    }

    QString status = tr("No media");
    if (media) {
        status = media->online() ? tr("Online") : tr("Offline");
    }

    auto* row = new QTreeWidgetItem(this);
    row->setText(0, displayName);
    row->setText(1, media ? QString::fromStdString(media->fileName()) : tr("--"));
    row->setText(2, QString::number(clip.frameCount()));
    row->setText(3, status);
    row->setData(0, Qt::UserRole, QString::fromStdString(clip.id().toString()));
}

void BinPanel::refresh()
{
    clear();
    if (!project_) {
        return;
    }
    for (const auto& clip : project_->rootBin().clips()) {
        if (clip) {
            addClipRow(*clip);
        }
    }
}

} // namespace mer::ui
