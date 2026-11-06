#include <crucible/gui/main_window.hpp>
#include <crucible/gui/stylesheet.hpp>

#include <crucible/core/logger.hpp>

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Crucible");
    app.setOrganizationName("Crucible");

    app.setStyleSheet(crucible::gui::load_stylesheet(
        QStringLiteral(":/style/dark.qss"),
        QStringLiteral(":/style/dark.vars")));

    crucible::LOG.error() << "hi 1";
    crucible::LOG.info() << "hi 2";
    crucible::LOG.warning() << "hi 3";
    crucible::LOG.debug() << "hi 4";
    crucible::LOG.debug() << "asjdfh adfhasdjfh 87y y13rkfj sef 7 abcdefghikjklmnopqursejhtkqjweb";

    crucible::gui::MainWindow main_window;
    main_window.show();

    return app.exec();
}
