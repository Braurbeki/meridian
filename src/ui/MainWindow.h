#pragma once
#include "playback/PlaybackEngine.h"
#include "project/Project.h"
#include "timeline/Timeline.h"
#include "util/Diagnostics.h"

#include <QMainWindow>
#include <memory>

class QTimer;

namespace mer::ui {

class BinPanel;
class ConsolePanel;
class InspectorPanel;
class TimelineRuler;
class TimelineView;
class TransportBar;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    bool openProject(const QString& path);

private slots:
    void onOpenProject();
    void onSaveProjectAs();
    void onPreferences();
    void onExportCutList();
    void onRebuildTimeline();
    void onAbout();

private:
    void buildUi();
    void buildMenus();
    void rebuildTimeline();
    void refreshPanels();
    void setStatus(const QString& text);

    std::unique_ptr<project::Project> project_;
    timeline::TimelinePtr             timeline_;
    playback::PlaybackEngine          engine_;
    util::DiagnosticSink              diagnostics_;

    TimelineView*   timelineView_ = nullptr;
    TimelineRuler*  ruler_        = nullptr;
    BinPanel*       binPanel_     = nullptr;
    InspectorPanel* inspector_    = nullptr;
    ConsolePanel*   console_      = nullptr;
    TransportBar*   transport_    = nullptr;
    QTimer*         tickTimer_    = nullptr;
};

} // namespace mer::ui
