#pragma once

#include <QDockWidget>
#include <QMainWindow>

namespace crucible::gui
{

class OpGraphPanel;

class MainWindow
    : public QMainWindow
{
    std::vector<QDockWidget*> panel_docks_;

    OpGraphPanel* op_graph_panel_;

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void createPanels();
    void createMenuBar();

    void createDemoGraph(); // TODO remove

    QDockWidget* createPanelDock(
        const QString& title,
        const QString& object_name,
        QWidget* content,
        Qt::DockWidgetArea area);
};

} // crucible::gui
