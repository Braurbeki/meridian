#pragma once
#include "util/Diagnostics.h"

#include <QPlainTextEdit>

namespace mer::ui {

/// Shows diagnostics collected while opening a project or building a
/// sequence. Cleared on every open.
class ConsolePanel : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit ConsolePanel(QWidget* parent = nullptr);

    void show(const util::DiagnosticSink& sink);
    void append(const util::Diagnostic& diagnostic);
};

} // namespace mer::ui
