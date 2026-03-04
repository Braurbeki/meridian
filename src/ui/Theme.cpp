#include "ui/Theme.h"

#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>

namespace mer::ui {
namespace {

QString stylesheet()
{
    return QStringLiteral(R"(
QWidget {
    background: #1c1f24;
    color: #d8dbe0;
}
QMainWindow, QMainWindow::separator {
    background: #16181c;
}
QMainWindow::separator {
    width: 4px; height: 4px;
    background: #16181c;
}
QMainWindow::separator:hover { background: #33383f; }

QMenuBar {
    background: #16181c;
    border-bottom: 1px solid #2a2e35;
    padding: 2px 4px;
}
QMenuBar::item { padding: 5px 11px; background: transparent; border-radius: 4px; }
QMenuBar::item:selected { background: #2b2f36; }
QMenu {
    background: #23262c;
    border: 1px solid #33383f;
    padding: 5px;
}
QMenu::item { padding: 6px 26px 6px 20px; border-radius: 4px; }
QMenu::item:selected { background: #34506b; }
QMenu::separator { height: 1px; background: #33383f; margin: 5px 8px; }

QDockWidget {
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
    font-size: 11px;
}
QDockWidget::title {
    background: #23262c;
    padding: 7px 10px;
    border-bottom: 1px solid #14161a;
    text-transform: uppercase;
    letter-spacing: 1px;
}

QTreeWidget, QTableWidget, QPlainTextEdit, QListWidget {
    background: #1a1d21;
    alternate-background-color: #1e2126;
    border: none;
    selection-background-color: #34506b;
    selection-color: #ffffff;
    outline: none;
}
QTreeWidget::item, QTableWidget::item { padding: 4px 2px; border: none; }
QTreeWidget::item:selected, QTableWidget::item:selected { background: #34506b; }

QHeaderView::section {
    background: #23262c;
    color: #8b919c;
    padding: 5px 8px;
    border: none;
    border-right: 1px solid #16181c;
    border-bottom: 1px solid #16181c;
    font-size: 11px;
}

QPushButton {
    background: #2b2f36;
    border: 1px solid #383d46;
    border-radius: 4px;
    padding: 6px 14px;
    min-height: 15px;
}
QPushButton:hover  { background: #333944; border-color: #454b56; }
QPushButton:pressed { background: #3f6fa0; border-color: #4b9fd5; }
QPushButton:disabled { color: #5d636e; background: #23262c; }

QSlider::groove:horizontal {
    height: 5px; background: #14161a;
    border: 1px solid #2a2e35; border-radius: 3px;
}
QSlider::sub-page:horizontal { background: #3f6fa0; border-radius: 3px; }
QSlider::handle:horizontal {
    width: 11px; margin: -6px 0;
    background: #c9ced6; border: 1px solid #14161a; border-radius: 3px;
}
QSlider::handle:horizontal:hover { background: #ffffff; }

QScrollBar:horizontal, QScrollBar:vertical { background: #16181c; border: none; }
QScrollBar:horizontal { height: 11px; }
QScrollBar:vertical   { width: 11px; }
QScrollBar::handle {
    background: #383d46; border-radius: 5px; min-width: 26px; min-height: 26px;
}
QScrollBar::handle:hover { background: #474d58; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0; height: 0; }
QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }

QStatusBar {
    background: #16181c;
    border-top: 1px solid #2a2e35;
    color: #8b919c;
}
QStatusBar::item { border: none; }

QGroupBox {
    border: 1px solid #33383f; border-radius: 5px;
    margin-top: 12px; padding-top: 10px;
}
QGroupBox::title { subcontrol-origin: margin; left: 10px; color: #8b919c; }

QSpinBox, QLineEdit, QComboBox {
    background: #14161a; border: 1px solid #33383f;
    border-radius: 4px; padding: 4px 7px;
}
QSpinBox:focus, QLineEdit:focus { border-color: #4b9fd5; }

QCheckBox::indicator {
    width: 14px; height: 14px;
    border: 1px solid #454b56; border-radius: 3px; background: #14161a;
}
QCheckBox::indicator:checked { background: #4b9fd5; border-color: #4b9fd5; }

QToolTip {
    background: #23262c; color: #d8dbe0;
    border: 1px solid #454b56; padding: 6px 8px;
}
)");
}

} // namespace

void applyTheme(QApplication& app)
{
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    QPalette p;
    p.setColor(QPalette::Window, theme::panel());
    p.setColor(QPalette::WindowText, theme::text());
    p.setColor(QPalette::Base, QColor(0x1a, 0x1d, 0x21));
    p.setColor(QPalette::AlternateBase, QColor(0x1e, 0x21, 0x26));
    p.setColor(QPalette::Text, theme::text());
    p.setColor(QPalette::Button, theme::raised());
    p.setColor(QPalette::ButtonText, theme::text());
    p.setColor(QPalette::Highlight, QColor(0x34, 0x50, 0x6b));
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::ToolTipBase, theme::surface());
    p.setColor(QPalette::ToolTipText, theme::text());
    p.setColor(QPalette::Disabled, QPalette::Text, theme::textFaint());
    p.setColor(QPalette::Disabled, QPalette::ButtonText, theme::textFaint());
    app.setPalette(p);

    QFont font = app.font();
    font.setPointSizeF(font.pointSizeF() * 0.95);
    app.setFont(font);

    app.setStyleSheet(stylesheet());
}

} // namespace mer::ui
