#pragma once

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

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void createDemoGraph();
};

} // crucible::gui
