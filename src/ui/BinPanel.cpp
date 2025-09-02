#include "ui/BinPanel.h"

#include "resolve/ResolverRegistry.h"

#include <QHeaderView>

namespace mer::ui {

BinPanel::BinPanel(QWidget* parent) : QTreeWidget(parent)
{
    setColumnCount(4);
    setHeaderLabels({tr("Name"), tr("File"), tr("Frames"), tr("Status")});
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);

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

    resolve::ResolveContext ctx;
    ctx.clipId       = clip.id();
    ctx.media        = media.get();
    ctx.editorialName = clip.name();
    ctx.clipMetadata = &clip.metadata();

    const auto& registry = resolve::ResolverRegistry::instance();

    auto* row = new QTreeWidgetItem(this);
    row->setText(0, QString::fromStdString(registry.resolve("clip.displayname", ctx)));
    row->setText(1, media ? QString::fromStdString(media->fileName()) : tr("--"));
    row->setText(2, QString::number(clip.frameCount()));
    row->setText(3, QString::fromStdString(registry.resolve("media.status", ctx)));
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
