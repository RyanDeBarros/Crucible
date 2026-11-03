#include <crucible/gui/main_window.hpp>
#include <crucible/gui/operation_graph_model.hpp>

#include <QtWidgets/qstatusbar.h>

namespace crucible::gui
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , operation_graph_model_(std::make_unique<OperationGraphModel>())
{
    setWindowTitle("Crucible");
    resize(1920, 1080);

    operation_graph_scene_ = std::make_unique<QtNodes::BasicGraphicsScene>(*operation_graph_model_);
    
    operation_graph_view_ = new QtNodes::GraphicsView(operation_graph_scene_.get());
    operation_graph_view_->setObjectName(QStringLiteral("operation_graph_view"));
    operation_graph_view_->setScaleRange(0.25, 2.0);

    setCentralWidget(operation_graph_view_);

    statusBar()->showMessage(QStringLiteral("Ready"));

    create_demo_graph();

    operation_graph_view_->zoomFitAll();
}

MainWindow::~MainWindow()
{
    // Don't delete operation_graph_view_: it is owned by QMainWindow::setCentralWidget()
    if (operation_graph_view_)
        operation_graph_view_->setScene(nullptr);
}

void MainWindow::create_demo_graph()
{
    using QtNodes::NodeRole;

    const auto source_node = operation_graph_model_->addNode(QStringLiteral("Text Source"));
    const auto transform_node = operation_graph_model_->addNode(QStringLiteral("Transform"));

    operation_graph_model_->setNodeData(source_node, NodeRole::Position, QPointF(-250.0, 0.0));
    operation_graph_model_->setNodeData(transform_node, NodeRole::Position, QPointF(100.0, 0.0));

    operation_graph_model_->addConnection({
        .outNodeId = source_node,
        .outPortIndex = 0,
        .inNodeId = transform_node,
        .inPortIndex = 0 });
}

} // crucible::gui
