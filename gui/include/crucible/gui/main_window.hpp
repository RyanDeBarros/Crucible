#pragma once

#include <QtNodes/internal/BasicGraphicsScene.hpp>
#include <QtNodes/internal/GraphicsView.hpp>

#include <QtWidgets/qmainwindow.h>

namespace crucible::gui
{

class OperationGraphModel;

class MainWindow
    : public QMainWindow
{
    std::unique_ptr<OperationGraphModel> operation_graph_model_;
    std::unique_ptr<QtNodes::BasicGraphicsScene> operation_graph_scene_;
    QtNodes::GraphicsView* operation_graph_view_ = nullptr;

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void create_demo_graph();
};

} // crucible::gui
