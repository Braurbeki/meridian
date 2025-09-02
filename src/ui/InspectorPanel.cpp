#include "ui/InspectorPanel.h"

#include "resolve/ResolverRegistry.h"

#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>

namespace mer::ui {

InspectorPanel::InspectorPanel(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    title_ = new QLabel(tr("No clip selected"), this);
    title_->setStyleSheet("font-weight: bold;");
    layout->addWidget(title_);

    table_ = new QTableWidget(this);
    table_->setColumnCount(3);
    table_->setHorizontalHeaderLabels({tr("Source"), tr("Field"), tr("Value")});
    table_->verticalHeader()->setVisible(false);
    table_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(table_);
}

void InspectorPanel::setProject(const project::Project* project)
{
    project_ = project;
    clear();
}

void InspectorPanel::clear()
{
    table_->setRowCount(0);
    title_->setText(tr("No clip selected"));
}

void InspectorPanel::addRow(const QString& source, const QString& key,
                            const QString& value, bool empty)
{
    const int row = table_->rowCount();
    table_->insertRow(row);
    table_->setItem(row, 0, new QTableWidgetItem(source));
    table_->setItem(row, 1, new QTableWidgetItem(key));

    auto* valueItem = new QTableWidgetItem(empty ? tr("(empty)") : value);
    if (empty) {
        valueItem->setForeground(QColor(0xc0, 0x80, 0x50));
    }
    table_->setItem(row, 2, valueItem);
}

void InspectorPanel::showClip(const QString& clipId)
{
    clear();
    if (!project_) {
        return;
    }

    const util::Uuid      id   = util::Uuid::fromString(clipId.toStdString());
    const project::ClipPtr clip = project_->findClip(id);
    if (!clip) {
        return;
    }

    const core::MediaSourcePtr media = project_->media().find(clip->mediaId());

    resolve::ResolveContext ctx;
    ctx.clipId       = clip->id();
    ctx.media        = media.get();
    ctx.editorialName = clip->name();
    ctx.clipMetadata = &clip->metadata();

    title_->setText(QString::fromStdString(
        resolve::ResolverRegistry::instance().resolve("clip.displayname", ctx)));

    addRow(tr("clip"), QStringLiteral("name"),
           QString::fromStdString(clip->name()), clip->name().empty());

    for (const auto& kv : clip->metadata()) {
        addRow(tr("clip"), QString::fromStdString(kv.first),
               QString::fromStdString(kv.second.asString()), !kv.second.hasContent());
    }

    if (media) {
        addRow(tr("file"), QStringLiteral("path"),
               QString::fromStdString(media->path()), media->path().empty());
        for (const auto& kv : media->metadata()) {
            addRow(tr("container"), QString::fromStdString(kv.first),
                   QString::fromStdString(kv.second.asString()),
                   !kv.second.hasContent());
        }
    }
}

} // namespace mer::ui
