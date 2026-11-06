#include <crucible/gui/main_window.hpp>
#include <crucible/gui/stylesheet.hpp>

#include <crucible/gui/log_panel/log_categories.hpp>

#include <crucible/core/logger.hpp>

#include <QApplication>

namespace crucible::gui
{

static int run(int argc, char* argv[])
{
    LOG.debug(kApp) << "Initializing Crucible application...";

    QApplication app(argc, argv);
    app.setApplicationName("Crucible");
    app.setOrganizationName("Crucible");

    LOG.debug(kApp) << "Loading stylesheet...";
    app.setStyleSheet(load_stylesheet(
        QStringLiteral(":/style/dark.qss"),
        QStringLiteral(":/style/dark.vars")));

    LOG.debug(kApp) << "Initializing window...";
    MainWindow main_window;
    main_window.show();

    LOG.debug(kApp) << "Starting Crucible application...";
    return app.exec();
}

} // crucible::gui

int main(int argc, char* argv[])
{
    return crucible::gui::run(argc, argv);
}
