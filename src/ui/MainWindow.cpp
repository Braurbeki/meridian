#include "ui/MainWindow.h"

#include "timeline/builder/TimelineBuilder.h"
#include "compat/v1/LegacyProjectImporter.h"
#include "plugins/ExportPlugin.h"
#include "plugins/PluginRegistry.h"
#include "core/MediaProbe.h"
#include "project/ProjectReader.h"
#include "project/ProjectWriter.h"
#include "ui/AppSettings.h"
#include "ui/Theme.h"
#include "ui/BinPanel.h"
#include "ui/ConsolePanel.h"
#include "ui/InspectorPanel.h"
#include "ui/MonitorView.h"
#include "ui/PreferencesDialog.h"
#include "ui/TimelineRuler.h"
#include "ui/TimelineView.h"
#include "ui/TransportBar.h"
#include "util/Log.h"

#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QSplitter>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>

namespace mer::ui {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle(tr("Meridian"));
    buildUi();
    buildMenus();

    const QByteArray geometry = AppSettings::instance().windowGeometry();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    } else {
        resize(1560, 900);
    }

    tickTimer_ = new QTimer(this);
    tickTimer_->setInterval(16);
    connect(tickTimer_, &QTimer::timeout, this, [this] { engine_.update(); });
    tickTimer_->start();

    setStatus(tr("Ready"));
}

MainWindow::~MainWindow()
{
    AppSettings::instance().setWindowGeometry(saveGeometry());
}

void MainWindow::buildUi()
{
    auto* splitter = new QSplitter(Qt::Vertical, this);
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(3);

    // --- program monitor --------------------------------------------------
    monitor_ = new MonitorView(splitter);
    monitor_->setFrameSource(&frames_);
    splitter->addWidget(monitor_);

    // --- transport + timeline ---------------------------------------------
    auto* lower       = new QWidget(splitter);
    auto* lowerLayout = new QVBoxLayout(lower);
    lowerLayout->setContentsMargins(0, 0, 0, 0);
    lowerLayout->setSpacing(0);

    transport_ = new TransportBar(lower);
    lowerLayout->addWidget(transport_);

    ruler_ = new TimelineRuler(lower);
    lowerLayout->addWidget(ruler_);

    auto* scroll = new QScrollArea(lower);
    scroll->setFrameShape(QFrame::NoFrame);
    timelineView_ = new TimelineView(scroll);
    timelineView_->setFrameSource(&frames_);
    scroll->setWidget(timelineView_);
    scroll->setWidgetResizable(true);
    lowerLayout->addWidget(scroll, 1);

    splitter->addWidget(lower);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);
    splitter->setSizes({460, 300});

    setCentralWidget(splitter);

    // --- docks ------------------------------------------------------------
    auto* binDock = new QDockWidget(tr("Bin"), this);
    binDock->setObjectName(QStringLiteral("binDock"));
    binPanel_ = new BinPanel(binDock);
    binDock->setWidget(binPanel_);
    addDockWidget(Qt::LeftDockWidgetArea, binDock);

    auto* inspectorDock = new QDockWidget(tr("Inspector"), this);
    inspectorDock->setObjectName(QStringLiteral("inspectorDock"));
    inspector_ = new InspectorPanel(inspectorDock);
    inspectorDock->setWidget(inspector_);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock);

    consoleDock_ = new QDockWidget(tr("Console"), this);
    consoleDock_->setObjectName(QStringLiteral("consoleDock"));
    console_ = new ConsolePanel(consoleDock_);
    consoleDock_->setWidget(console_);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock_);

    // Stays out of the way until something has been reported.
    consoleDock_->hide();

    // Docks default to an equal share of the window, which buries the
    // timeline under an empty console. Give them workable proportions.
    resizeDocks({binDock, inspectorDock}, {330, 340}, Qt::Horizontal);
    resizeDocks({consoleDock_}, {130}, Qt::Vertical);

    // --- wiring -----------------------------------------------------------
    connect(binPanel_, &BinPanel::clipSelected, inspector_, &InspectorPanel::showClip);

    connect(timelineView_, &TimelineView::playheadMoved, this, [this](qint64 frame) {
        engine_.seek(frame);
        ruler_->setPlayhead(frame);
        transport_->setPosition(frame);
        monitor_->setPlayhead(frame);
    });
    connect(timelineView_, &TimelineView::segmentClicked, this,
            [this](const QString& segmentId) {
                if (!timeline_) {
                    return;
                }
                const auto id = util::Uuid::fromString(segmentId.toStdString());
                if (const auto* segment = timeline_->findSegment(id)) {
                    inspector_->showClip(
                        QString::fromStdString(segment->clipId().toString()));
                }
            });

    connect(transport_, &TransportBar::playPauseRequested, this, [this] {
        if (engine_.state() == playback::TransportState::Playing) {
            engine_.pause();
            transport_->setPlaying(false);
        } else {
            engine_.play();
            transport_->setPlaying(true);
        }
    });
    connect(transport_, &TransportBar::stopRequested, this, [this] {
        engine_.stop();
        transport_->setPlaying(false);
    });
    connect(transport_, &TransportBar::stepRequested, this,
            [this](int frames) { engine_.step(frames); });
    connect(transport_, &TransportBar::seekRequested, this,
            [this](qint64 frame) { engine_.seek(frame); });

    engine_.onFrame([this](std::int64_t frame) {
        timelineView_->setPlayhead(frame);
        ruler_->setPlayhead(frame);
        transport_->setPosition(frame);
        monitor_->setPlayhead(frame);
    });
}

void MainWindow::buildMenus()
{
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open Project..."), QKeySequence::Open, this,
                        &MainWindow::onOpenProject);
    fileMenu->addAction(tr("Save Project &As..."), QKeySequence::SaveAs, this,
                        &MainWindow::onSaveProjectAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Export Cut List..."), this, &MainWindow::onExportCutList);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Quit"), QKeySequence::Quit, qApp, &QApplication::quit);

    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Preferences..."), this, &MainWindow::onPreferences);

    QMenu* sequenceMenu = menuBar()->addMenu(tr("&Sequence"));
    sequenceMenu->addAction(tr("&Rebuild"), QKeySequence(Qt::Key_F5), this,
                            &MainWindow::onRebuildTimeline);

    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&Fit Sequence"), QKeySequence(Qt::Key_Backslash), this,
                        &MainWindow::onFitSequence);
    QAction* overlay = viewMenu->addAction(tr("Monitor &Overlay"), this,
                                           &MainWindow::onToggleOverlay);
    overlay->setCheckable(true);
    overlay->setChecked(true);
    viewMenu->addSeparator();
    viewMenu->addAction(consoleDock_->toggleViewAction());

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About Meridian"), this, &MainWindow::onAbout);
}

void MainWindow::setStatus(const QString& text)
{
    statusBar()->showMessage(text);
}

void MainWindow::onOpenProject()
{
    const QString dir = AppSettings::instance().lastProjectDir();
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Open Project"), dir,
        tr("Meridian projects (*.mrp *.mrpx);;All files (*)"));
    if (!path.isEmpty()) {
        openProject(path);
    }
}

bool MainWindow::openProject(const QString& path)
{
    diagnostics_.clear();
    frames_.clear();

    const std::string stdPath = path.toStdString();

    if (compat::v1::LegacyProjectImporter::canImport(stdPath)) {
        const compat::v1::LegacyProjectImporter importer;
        project_ = importer.import(stdPath, diagnostics_);
    } else {
        const project::ProjectReader reader;
        project_ = reader.read(stdPath, diagnostics_);
    }

    showDiagnostics();

    if (!project_) {
        QMessageBox::warning(this, tr("Open Project"),
                             tr("Could not open %1. See the Console panel.").arg(path));
        return false;
    }

    if (AppSettings::instance().probeOnOpen()) {
        project_->media().refreshAll(diagnostics_);
        showDiagnostics();
    }

    AppSettings::instance().setLastProjectDir(QFileInfo(path).absolutePath());
    setWindowTitle(tr("Meridian - %1").arg(QString::fromStdString(project_->name())));

    rebuildTimeline();
    refreshPanels();

    // Geometry is not final until the event loop has run once.
    QTimer::singleShot(0, this, &MainWindow::onFitSequence);

    setStatus(tr("Opened %1").arg(path));
    return true;
}

void MainWindow::rebuildTimeline()
{
    if (!project_ || project_->timelines().empty()) {
        timeline_.reset();
        timelineView_->setTimeline(nullptr);
        monitor_->setTimeline(nullptr);
        return;
    }

    const AppSettings& settings = AppSettings::instance();

    timeline::SegmentDecorator::Settings decoratorSettings;
    decoratorSettings.truncateLabels = settings.truncateLabels();
    decoratorSettings.maxLabelChars  = settings.maxLabelChars();

    timeline::TimelineBuilder builder(*project_, this);
    builder.setDecoratorSettings(decoratorSettings);

    timeline_ = builder.build(project_->timelines().front(), diagnostics_);

    timelineView_->setTimeline(timeline_);
    monitor_->setTimeline(timeline_);
    ruler_->setRate(timeline_->rate());
    ruler_->setDuration(timeline_->duration());
    ruler_->setPixelsPerFrame(timelineView_->pixelsPerFrame());
    transport_->setRate(timeline_->rate());
    transport_->setDuration(timeline_->duration());
    engine_.setTimeline(timeline_);

    showDiagnostics();
}

void MainWindow::refreshPanels()
{
    binPanel_->setProject(project_.get());
    inspector_->setProject(project_.get());
}

void MainWindow::onRebuildTimeline()
{
    if (!project_) {
        return;
    }
    rebuildTimeline();
    refreshPanels();
    setStatus(tr("Sequence rebuilt"));
}

void MainWindow::onSaveProjectAs()
{
    if (!project_) {
        return;
    }
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Save Project As"), AppSettings::instance().lastProjectDir(),
        tr("Meridian projects (*.mrp)"));
    if (path.isEmpty()) {
        return;
    }

    const project::ProjectWriter writer;
    if (writer.write(*project_, path.toStdString(), diagnostics_)) {
        setStatus(tr("Saved %1").arg(path));
    }
    showDiagnostics();
}

void MainWindow::onExportCutList()
{
    if (!timeline_) {
        return;
    }
    const auto plugin = plugins::PluginRegistry::instance().create(
        "com.meridian.export.cutlist");
    auto* exporter = dynamic_cast<plugins::ExportPlugin*>(plugin.get());
    if (!exporter) {
        QMessageBox::warning(this, tr("Export"), tr("Cut list plugin is not available."));
        return;
    }

    const QString path = QFileDialog::getSaveFileName(
        this, tr("Export Cut List"), QString(), tr("Text files (*.txt)"));
    if (path.isEmpty()) {
        return;
    }
    if (exporter->exportTimeline(*timeline_, path.toStdString())) {
        setStatus(tr("Exported cut list to %1").arg(path));
    }
}

void MainWindow::onPreferences()
{
    PreferencesDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted && project_) {
        rebuildTimeline();
        refreshPanels();
    }
}

void MainWindow::showDiagnostics()
{
    console_->show(diagnostics_);
    if (diagnostics_.size() > 0) {
        consoleDock_->show();
    }
}

void MainWindow::onFitSequence()
{
    timelineView_->zoomToFit();
    ruler_->setPixelsPerFrame(timelineView_->pixelsPerFrame());
}

void MainWindow::onToggleOverlay()
{
    monitor_->setShowOverlay(!monitor_->showOverlay());
}

void MainWindow::onAbout()
{
    QMessageBox::about(
        this, tr("About Meridian"),
        tr("<b>Meridian</b> %1<br><br>Sequence review and conform tool."
           "<br><br>Media backend: %2")
            .arg(QStringLiteral(MERIDIAN_VERSION_STRING),
                 QString::fromStdString(core::MediaProbe::ffmpegVersionString())));
}

} // namespace mer::ui
