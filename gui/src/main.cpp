#include <crucible/gui/main_window.hpp>

#include <crucible/gui/log_panel/log_categories.hpp>

#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/util/stylesheet.hpp>

#include <crucible/core/logging/logger.hpp>

#include <QApplication>





#include <crucible/core/typing/storage.hpp>

namespace crucible::gui
{

static int run(int argc, char* argv[])
{
    LOG.info(kApp) << reflector::erase_type<core::typing::IntPayload>().name();
    LOG.info(kApp) << reflector::erase_type<std::vector<core::typing::IntPayload>>().name();

    const auto& tree = reflector::compiler_type_tree<std::vector<core::typing::IntPayload>>();

    LOG.debug(kApp) << "Initializing Crucible application...";

    QApplication app(argc, argv);
    app.setApplicationName("Crucible");
    app.setOrganizationName("Ryan de Barros");

    LOG.debug(kApp) << "Loading stylesheet...";
    app.setStyleSheet(loadStylesheet(Stylesheet::Dark));

    PREFS.loadSettings();

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
