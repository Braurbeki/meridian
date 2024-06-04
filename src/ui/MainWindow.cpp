#include "ui/MainWindow.h"

#include "timeline/builder/TimelineBuilder.h"
#include "compat/v1/LegacyProjectImporter.h"
#include "plugins/ExportPlugin.h"
#include "plugins/PluginRegistry.h"
#include "project/ProjectReader.h"
#include "project/ProjectWriter.h"
#include "ui/AppSettings.h"
#include "ui/BinPanel.h"
#include "ui/ConsolePanel.h"
#include "ui/InspectorPanel.h"
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
        resize(1280, 760);
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
    auto* central = new QWidget(this);
    auto* layout  = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    ruler_ = new TimelineRuler(central);
    layout->addWidget(ruler_);

    auto* scroll = new QScrollArea(central);
    timelineView_ = new TimelineView(scroll);
    scroll->setWidget(timelineView_);
    scroll->setWidgetResizable(true);
    layout->addWidget(scroll, 1);

    transport_ = new TransportBar(central);
    layout->addWidget(transport_);

    setCentralWidget(central);

    auto* binDock = new QDockWidget(tr("Bin"), this);
    binPanel_     = new BinPanel(binDock);
    binDock->setWidget(binPanel_);
    addDockWidget(Qt::LeftDockWidgetArea, binDock);

    auto* inspectorDock = new QDockWidget(tr("Inspector"), this);
    inspector_          = new InspectorPanel(inspectorDock);
    inspectorDock->setWidget(inspector_);
    addDockWidget(Qt::RightDockWidgetArea, inspectorDock);

    auto* consoleDock = new QDockWidget(tr("Console"), this);
    console_          = new ConsolePanel(consoleDock);
    consoleDock->setWidget(console_);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock);

    connect(binPanel_, &BinPanel::clipSelected, inspector_, &InspectorPanel::showClip);

    connect(timelineView_, &TimelineView::playheadMoved, this, [this](qint64 frame) {
        engine_.seek(frame);
        ruler_->setPlayhead(frame);
        transport_->setPosition(frame);
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

    QMenu* sequenceMenu = menuBar()->addMenu(tr("&Sequence"));
    sequenceMenu->addAction(tr("&Rebuild"), QKeySequence(Qt::Key_F5), this,
                            &MainWindow::onRebuildTimeline);

    QMenu* editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Preferences..."), this, &MainWindow::onPreferences);

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

    const std::string stdPath = path.toStdString();

    if (compat::v1::LegacyProjectImporter::canImport(stdPath)) {
        const compat::v1::LegacyProjectImporter importer;
        project_ = importer.import(stdPath, diagnostics_);
    } else {
        const project::ProjectReader reader;
        project_ = reader.read(stdPath, diagnostics_);
    }

    console_->show(diagnostics_);

    if (!project_) {
        QMessageBox::warning(this, tr("Open Project"),
                             tr("Could not open %1. See the Console panel.").arg(path));
        return false;
    }

    if (AppSettings::instance().probeOnOpen()) {
        project_->media().refreshAll(diagnostics_);
        console_->show(diagnostics_);
    }

    AppSettings::instance().setLastProjectDir(QFileInfo(path).absolutePath());
    setWindowTitle(tr("Meridian - %1").arg(QString::fromStdString(project_->name())));

    rebuildTimeline();
    refreshPanels();
    setStatus(tr("Opened %1").arg(path));
    return true;
}

void MainWindow::rebuildTimeline()
{
    if (!project_ || project_->timelines().empty()) {
        timeline_.reset();
        timelineView_->setTimeline(nullptr);
        return;
    }

    const AppSettings& settings = AppSettings::instance();

    timeline::SegmentDecorator::Settings decoratorSettings;
    decoratorSettings.truncateLabels = settings.truncateLabels();
    decoratorSettings.maxLabelChars  = settings.maxLabelChars();

    timeline::TimelineBuilder builder(*project_);
    builder.setDecoratorSettings(decoratorSettings);

    timeline_ = builder.build(project_->timelines().front(), diagnostics_);

    timelineView_->setTimeline(timeline_);
    ruler_->setRate(timeline_->rate());
    ruler_->setDuration(timeline_->duration());
    transport_->setRate(timeline_->rate());
    transport_->setDuration(timeline_->duration());
    engine_.setTimeline(timeline_);

    console_->show(diagnostics_);
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
    console_->show(diagnostics_);
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

void MainWindow::onAbout()
{
    QMessageBox::about(
        this, tr("About Meridian"),
        tr("<b>Meridian</b> %1<br><br>Sequence review and conform tool.")
            .arg(QStringLiteral(MERIDIAN_VERSION_STRING)));
}

} // namespace mer::ui
