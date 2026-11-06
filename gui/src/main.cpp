#include <crucible/gui/main_window.hpp>
#include <crucible/gui/stylesheet.hpp>

#include <crucible/core/logger.hpp>

#include <QApplication>

namespace crucible::gui
{

static int run(int argc, char* argv[])
{
    LOG.debug() << "Initializing Crucible application...";

    QApplication app(argc, argv);
    app.setApplicationName("Crucible");
    app.setOrganizationName("Crucible");

    LOG.debug() << "Loading stylesheet...";
    app.setStyleSheet(load_stylesheet(
        QStringLiteral(":/style/dark.qss"),
        QStringLiteral(":/style/dark.vars")));

    LOG.debug() << "Initializing window...";
    MainWindow main_window;
    main_window.show();

    LOG.debug() << "Starting Crucible application...";
    return app.exec();
}

} // crucible::gui

int main(int argc, char* argv[])
{
    return crucible::gui::run(argc, argv);
}
