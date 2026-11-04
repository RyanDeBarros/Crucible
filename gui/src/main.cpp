#include <crucible/gui/main_window.hpp>

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Crucible");
    app.setOrganizationName("Crucible");

    crucible::gui::MainWindow main_window;
    main_window.show();

    return app.exec();
}
