#pragma once

#include <QDockWidget>
#include <QMainWindow>

namespace crucible::gui
{

class MainWindow
    : public QMainWindow
{
    std::vector<QDockWidget*> panel_docks_;

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void createPanels();
    void createMenuBar();

    QDockWidget* createPanelDock(
        const QString& title,
        const QString& object_name,
        QWidget* content,
        Qt::DockWidgetArea area);
};

} // crucible::gui
