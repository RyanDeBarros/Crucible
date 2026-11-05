#pragma once

#include <QtNodes/internal/AbstractGraphModel.hpp>

#include <QPointF>
#include <QSize>

namespace crucible::gui
{

class OpGraphModel
    : public QtNodes::AbstractGraphModel
{
public:
    struct NodeGeometry
    {
        QPointF position{};
        QSize size{};
    };

private:
    QtNodes::NodeId next_node_id_ = { 0 };

    std::unordered_set<QtNodes::NodeId> node_ids_;
    std::unordered_set<QtNodes::ConnectionId> connection_ids_;

    std::unordered_map<QtNodes::NodeId, QString> node_captions_;
    mutable std::unordered_map<QtNodes::NodeId, NodeGeometry> node_geometry_;

public:
    OpGraphModel() = default;
    ~OpGraphModel() override = default;

    std::unordered_set<QtNodes::NodeId> allNodeIds() const override;
    std::unordered_set<QtNodes::ConnectionId> allConnectionIds(QtNodes::NodeId node_id) const override;

    std::unordered_set<QtNodes::ConnectionId> connections(
        QtNodes::NodeId node_id,
        QtNodes::PortType port_type,
        QtNodes::PortIndex port_index) const override;

    bool connectionPossible(QtNodes::ConnectionId connection_id) const override;

    bool nodeExists(QtNodes::NodeId node_id) const override;
    QtNodes::NodeId addNode(QString node_type = QString()) override;
    bool deleteNode(QtNodes::NodeId node_id) override;

    QtNodes::NodeId newNodeId() override;
    QJsonObject saveNode(QtNodes::NodeId node_id) const override;
    void loadNode(const QJsonObject& node_json) override;

    bool connectionExists(QtNodes::ConnectionId connection_id) const override;
    void addConnection(QtNodes::ConnectionId connection_id) override;
    bool deleteConnection(QtNodes::ConnectionId connection_id) override;

    QVariant nodeData(QtNodes::NodeId node_id, QtNodes::NodeRole role) const override;
    bool setNodeData(QtNodes::NodeId node_id, QtNodes::NodeRole role, QVariant value) override;
        
    QVariant portData(
        QtNodes::NodeId node_id,
        QtNodes::PortType port_type,
        QtNodes::PortIndex port_index,
        QtNodes::PortRole role) const override;
        
    bool setPortData(
        QtNodes::NodeId node_id,
        QtNodes::PortType port_type,
        QtNodes::PortIndex port_index,
        const QVariant& value,
        QtNodes::PortRole role = QtNodes::PortRole::Data) override;

public:
    QString defaultNodeName() const;
};

} // crucible::gui
