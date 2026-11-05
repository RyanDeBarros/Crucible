#pragma once

#include <QDockWidget>
#include <QMainWindow>

namespace crucible::gui
{

class OpGraphModel;
class OpGraphScene;
class OpGraphView;

class MainWindow
    : public QMainWindow
{
    std::unique_ptr<OpGraphModel> op_graph_model_;
    std::unique_ptr<OpGraphScene> op_graph_scene_;
    OpGraphView* op_graph_view_ = nullptr;

    std::vector<QDockWidget*> panel_docks_;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void createPanels();
    void createMenuBar();

    void createDemoGraph(); // TODO remove

    QDockWidget* createPanelDock(
        QString const& title,
        QString const& object_name,
        QWidget* content,
        Qt::DockWidgetArea area);
};

} // crucible::gui
