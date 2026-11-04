#include <crucible/gui/op_graph/op_graph_view.hpp>

#include <crucible/gui/op_graph/op_graph_model.hpp>
#include <crucible/gui/op_graph/op_graph_scene.hpp>

#include <QContextMenuEvent>
#include <QMenu>

namespace crucible::gui
{

using QtNodes::GraphicsView;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeRole;

OpGraphView::OpGraphView(OpGraphScene& scene, OpGraphModel& model)
    : GraphicsView(&scene)
    , model_(model)
    , scene_(scene)
{
    setObjectName(QStringLiteral("op_graph_view"));
    setScaleRange(0.25, 2.0);
}

void OpGraphView::contextMenuEvent(QContextMenuEvent* event)
{
    auto* item = itemAt(event->pos());

    auto* current = item;
    NodeGraphicsObject* node = nullptr;

    while (current && !node)
    {
        node = qgraphicsitem_cast<NodeGraphicsObject*>(current);
        current = current->parentItem();
    }

    if (node)
        contextMenuNode(event, node);
    else if (!item)
        contextMenuVoid(event);
    else
        GraphicsView::contextMenuEvent(event);
}

void OpGraphView::contextMenuNode(QContextMenuEvent* event, QtNodes::NodeGraphicsObject* node)
{
    const auto node_id = node->nodeId();

    if (!node->isSelected())
    {
        scene_.clearSelection();
        node->setSelected(true);
    }

    const bool plural_nodes = scene_.selectedNodes().size() > 1;

    QMenu menu(this);

    auto* copy_action = menu.addAction(
        plural_nodes ? QStringLiteral("Copy Nodes") : QStringLiteral("Copy Node"));

    auto* cut_action = menu.addAction(
        plural_nodes ? QStringLiteral("Cut Nodes") : QStringLiteral("Cut Node"));

    menu.addSeparator();

    auto* delete_action = menu.addAction(
        plural_nodes ? QStringLiteral("Delete Nodes") : QStringLiteral("Delete Node"));

    const auto* selected_action = menu.exec(event->globalPos());

    if (selected_action == copy_action)
        onCopySelectedObjects();
    else if (selected_action == cut_action)
    {
        onCopySelectedObjects();
        onDeleteSelectedObjects();
    }
    else if (selected_action == delete_action)
        onDeleteSelectedObjects();

    event->accept();
}

void OpGraphView::contextMenuVoid(QContextMenuEvent* event)
{
    QMenu menu(this);

    auto* create_action = menu.addAction(QStringLiteral("Create Node"));
    auto* paste_action = menu.addAction(QStringLiteral("Paste Node(s)"));

    const auto* selected_action = menu.exec(event->globalPos());

    if (selected_action == create_action)
    {
        const auto node_id = model_.addNode(QStringLiteral("Node"));
        model_.setNodeData(node_id, NodeRole::Position, mapToScene(event->pos()));
    }
    else if (selected_action == paste_action)
        onPasteObjects();

    event->accept();
}

} // crucible::gui
