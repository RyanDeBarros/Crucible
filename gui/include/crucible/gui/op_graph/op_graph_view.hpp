#pragma once

#include <QtNodes/internal/GraphicsView.hpp>

namespace QtNodes
{

class NodeGraphicsObject;

}

namespace crucible::gui
{

class OpGraphModel;
class OpGraphScene;

class OpGraphView
    : public QtNodes::GraphicsView
{
    OpGraphModel& model_;
    OpGraphScene& scene_;

public:
    explicit OpGraphView(OpGraphScene& scene, OpGraphModel& model);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void contextMenuNode(QContextMenuEvent* event, QtNodes::NodeGraphicsObject* node);
    void contextMenuVoid(QContextMenuEvent* event);
};

} // crucible::gui
