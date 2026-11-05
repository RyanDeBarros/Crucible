#include <crucible/gui/main_window.hpp>
#include <crucible/gui/stylesheet.hpp>

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Crucible");
    app.setOrganizationName("Crucible");

    app.setStyleSheet(crucible::gui::load_stylesheet(
        QStringLiteral(":/style/dark.qss"),
        QStringLiteral(":/style/dark.vars")));

    crucible::gui::MainWindow main_window;
    main_window.show();

    return app.exec();
}
