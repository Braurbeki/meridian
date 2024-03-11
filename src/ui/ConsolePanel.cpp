#include "ui/ConsolePanel.h"

namespace mer::ui {
namespace {

QString severityTag(util::Severity s)
{
    switch (s) {
    case util::Severity::Info:    return QStringLiteral("info");
    case util::Severity::Warning: return QStringLiteral("warn");
    case util::Severity::Error:   return QStringLiteral("error");
    }
    return QStringLiteral("?");
}

} // namespace

ConsolePanel::ConsolePanel(QWidget* parent) : QPlainTextEdit(parent)
{
    setReadOnly(true);
    setMaximumBlockCount(2000);
    setPlaceholderText(tr("Diagnostics from the last open will appear here."));
}

void ConsolePanel::append(const util::Diagnostic& diagnostic)
{
    QString line = QStringLiteral("[%1] %2: %3")
                       .arg(severityTag(diagnostic.severity),
                            QString::fromStdString(diagnostic.code),
                            QString::fromStdString(diagnostic.message));
    if (!diagnostic.context.empty()) {
        line += QStringLiteral("  (%1)").arg(QString::fromStdString(diagnostic.context));
    }
    appendPlainText(line);
}

void ConsolePanel::show(const util::DiagnosticSink& sink)
{
    clear();
    for (const auto& entry : sink.entries()) {
        append(entry);
    }
}

} // namespace mer::ui
