#include <crucible/gui/main_window.hpp>

#include <crucible/gui/op_graph/op_graph_model.hpp>
#include <crucible/gui/op_graph/op_graph_scene.hpp>
#include <crucible/gui/op_graph/op_graph_view.hpp>

#include <QScreen>
#include <QStatusBar>

namespace crucible::gui
{

using QtNodes::BasicGraphicsScene;
using QtNodes::NodeId;
using QtNodes::NodeRole;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Crucible");
    resize(1920, 1080);
    move(screen()->geometry().center() - frameGeometry().center());

    op_graph_model_ = std::make_unique<OpGraphModel>();
    op_graph_scene_ = std::make_unique<OpGraphScene>(*op_graph_model_);
    op_graph_view_ = new OpGraphView(*op_graph_scene_, *op_graph_model_);

    setCentralWidget(op_graph_view_);

    statusBar()->showMessage(QStringLiteral("Ready"));

    createDemoGraph();

    op_graph_view_->zoomFitAll();
}

MainWindow::~MainWindow()
{
    // Don't delete op_graph_view_: it is owned by QMainWindow::setCentralWidget()
    if (op_graph_view_)
        op_graph_view_->setScene(nullptr);
}

void MainWindow::createDemoGraph()
{
    const auto source_node = op_graph_model_->addNode(QStringLiteral("Text Source"));
    const auto transform_node = op_graph_model_->addNode(QStringLiteral("Transform"));

    op_graph_model_->setNodeData(source_node, NodeRole::Position, QPointF(-250.0, 0.0));
    op_graph_model_->setNodeData(transform_node, NodeRole::Position, QPointF(100.0, 0.0));

    op_graph_model_->addConnection({
        .outNodeId = source_node,
        .outPortIndex = 0,
        .inNodeId = transform_node,
        .inPortIndex = 0 });
}

} // crucible::gui
