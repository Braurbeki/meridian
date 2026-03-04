#include "CommandLine.h"

#include "ui/MainWindow.h"
#include "ui/Theme.h"
#include "util/Log.h"

#include <QApplication>

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    mer::util::initLoggingFromEnv();

    const std::vector<std::string> args(argv, argv + argc);
    const mer::app::Options        options = mer::app::parseCommandLine(args);

    if (options.showHelp) {
        std::cout << mer::app::usageText();
        return 0;
    }
    if (options.showVersion) {
        std::cout << "Meridian " << MERIDIAN_VERSION_STRING << "\n";
        return 0;
    }
    if (options.listResolvers) {
        return mer::app::runListResolvers();
    }
    if (options.headless) {
        if (options.projectPath.empty()) {
            std::cerr << "--dump-labels requires a project path\n";
            return 2;
        }
        return mer::app::runDumpLabels(options.projectPath);
    }

    QApplication application(argc, argv);
    QApplication::setApplicationName(QStringLiteral("Meridian"));
    QApplication::setApplicationVersion(QStringLiteral(MERIDIAN_VERSION_STRING));
    mer::ui::applyTheme(application);

    mer::ui::MainWindow window;
    window.show();

    if (!options.projectPath.empty()) {
        window.openProject(QString::fromStdString(options.projectPath));
    }

    return application.exec();
}
