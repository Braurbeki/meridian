#pragma once
#include "project/Project.h"

#include <QTreeWidget>

namespace mer::ui {

/// Lists the clips in the project's bins.
///
/// Captions come from the "clip.displayname" resolver so that the bin, the
/// Inspector and the conform report always agree with each other.
class BinPanel : public QTreeWidget {
    Q_OBJECT

public:
    explicit BinPanel(QWidget* parent = nullptr);

    void setProject(const project::Project* project);
    void refresh();

signals:
    void clipSelected(const QString& clipId);

private:
    void addClipRow(const project::Clip& clip);

    const project::Project* project_ = nullptr;
};

} // namespace mer::ui
