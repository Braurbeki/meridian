#pragma once
#include "project/Project.h"

#include <QWidget>

class QTableWidget;
class QLabel;

namespace mer::ui {

/// Shows every metadata field on the selected clip, clip-level entries first,
/// then the container tags underneath. Useful when a caption is not what a
/// user expects: this panel shows exactly what the resolvers see.
class InspectorPanel : public QWidget {
    Q_OBJECT

public:
    explicit InspectorPanel(QWidget* parent = nullptr);

    void setProject(const project::Project* project);
    void showClip(const QString& clipId);
    void clear();

private:
    void addRow(const QString& source, const QString& key, const QString& value,
                bool empty);

    const project::Project* project_ = nullptr;
    QLabel*                 title_   = nullptr;
    QTableWidget*           table_   = nullptr;
};

} // namespace mer::ui
