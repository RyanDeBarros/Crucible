#include <crucible/gui/operation_graph_model.hpp>

#include <QtNodes/internal/ConnectionIdUtils.hpp>
#include <QtNodes/internal/StyleCollection.hpp>

namespace crucible::gui
{

using QtNodes::ConnectionId;
using QtNodes::ConnectionPolicy;
using QtNodes::NodeId;
using QtNodes::NodeRole;
using QtNodes::PortIndex;
using QtNodes::PortRole;
using QtNodes::PortType;
using QtNodes::StyleCollection;

std::unordered_set<NodeId> OperationGraphModel::allNodeIds() const
{
    return node_ids_;
}

std::unordered_set<ConnectionId> OperationGraphModel::allConnectionIds(NodeId node_id) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(
        connection_ids_.begin(),
        connection_ids_.end(),
        std::inserter(result, result.end()),
        [node_id](const ConnectionId& connection) {
            return connection.inNodeId == node_id || connection.outNodeId == node_id;
        });

    return result;
}

std::unordered_set<ConnectionId> OperationGraphModel::connections(
    NodeId node_id,
    PortType port_type,
    PortIndex port_index) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(
        connection_ids_.begin(),
        connection_ids_.end(),
        std::inserter(result, result.end()),
        [node_id, port_type, port_index](const ConnectionId& connection) {
            return QtNodes::getNodeId(port_type, connection) == node_id
                && QtNodes::getPortIndex(port_type, connection) == port_index;
        });

    return result;
}

bool OperationGraphModel::connectionPossible(ConnectionId connection_id) const
{
    return !connection_ids_.contains(connection_id);
}

bool OperationGraphModel::nodeExists(NodeId node_id) const
{
    return node_ids_.contains(node_id);
}

NodeId OperationGraphModel::addNode(QString node_type)
{
    const NodeId node_id = newNodeId();
    node_ids_.insert(node_id);
    node_captions_[node_id] = node_type.isEmpty() ? QStringLiteral("Node") : node_type;

    Q_EMIT nodeCreated(node_id);
    return node_id;
}
    
bool OperationGraphModel::deleteNode(NodeId node_id)
{
    if (!nodeExists(node_id))
        return false;

    const auto node_connections = allConnectionIds(node_id);
    for (const auto& connection : node_connections)
        deleteConnection(connection);

    node_ids_.erase(node_id);
    node_captions_.erase(node_id);
    node_geometry_.erase(node_id);

    Q_EMIT nodeDeleted(node_id);

    return true;
}

NodeId OperationGraphModel::newNodeId()
{
    return next_node_id_++;
}

QJsonObject OperationGraphModel::saveNode(NodeId node_id) const
{
    QJsonObject json;

    json["id"] = static_cast<qint64>(node_id);
    json["caption"] = node_captions_.at(node_id);

    const QPointF position = node_geometry_[node_id].position;

    QJsonObject position_json;
    position_json["x"] = position.x();
    position_json["y"] = position.y();

    json["position"] = position_json;

    return json;
}

void OperationGraphModel::loadNode(const QJsonObject& node_json)
{
    const NodeId node_id = static_cast<NodeId>(node_json["id"].toInteger());

    next_node_id_ = std::max(next_node_id_, node_id + 1);

    node_ids_.insert(node_id);
    node_captions_[node_id] = node_json["caption"].toString(QStringLiteral("Node"));

    Q_EMIT nodeCreated(node_id);

    const QJsonObject position_json = node_json["position"].toObject();

    setNodeData(
        node_id,
        NodeRole::Position,
        QPointF(position_json["x"].toDouble(), position_json["y"].toDouble()));
}

bool OperationGraphModel::connectionExists(ConnectionId connection_id) const
{
    return connection_ids_.contains(connection_id);
}

void OperationGraphModel::addConnection(ConnectionId connection_id)
{
    if (!connectionPossible(connection_id))
        return;

    connection_ids_.insert(connection_id);
    Q_EMIT connectionCreated(connection_id);
}

bool OperationGraphModel::deleteConnection(ConnectionId connection_id)
{
    if (connection_ids_.erase(connection_id) == 0)
        return false;

    Q_EMIT connectionDeleted(connection_id);
    return true;
}

QVariant OperationGraphModel::nodeData(NodeId node_id, NodeRole role) const
{

    switch (role)
    {
    case NodeRole::Type:
        return QStringLiteral("default");

    case NodeRole::Position:
        return node_geometry_[node_id].position;

    case NodeRole::Size:
        return node_geometry_[node_id].size;

    case NodeRole::CaptionVisible:
        return true;

    case NodeRole::Caption:
        return node_captions_.contains(node_id) ? node_captions_.at(node_id) : QStringLiteral("Node");

    case NodeRole::Style:
        return StyleCollection::nodeStyle().toJson().toVariantMap();

    case NodeRole::InPortCount:
        return 1u;

    case NodeRole::OutPortCount:
        return 1u;

    case NodeRole::InternalData:
    case NodeRole::Widget:
        return {};
    }

    return {};
}

bool OperationGraphModel::setNodeData(NodeId node_id, NodeRole role, QVariant value)
{
    if (!nodeExists(node_id))
        return false;

    switch (role)
    {
    case NodeRole::Position:
        node_geometry_[node_id].position = value.toPointF();
        Q_EMIT nodePositionUpdated(node_id);
        return true;

    case NodeRole::Size:
        node_geometry_[node_id].size = value.toSize();
        return true;

    default:
        return false;
    }
}

QVariant OperationGraphModel::portData(
    NodeId node_id,
    PortType port_type,
    PortIndex port_index,
    PortRole role) const
{
    Q_UNUSED(node_id);
    Q_UNUSED(port_index);

    switch (role)
    {
    case PortRole::Data:
    case PortRole::DataType:
        return {};

    case PortRole::ConnectionPolicyRole:
        return QVariant::fromValue(ConnectionPolicy::One);

    case PortRole::CaptionVisible:
        return true;

    case PortRole::Caption:
        return port_type == PortType::In ? QStringLiteral("Input") : QStringLiteral("Output");
    }

    return {};
}

bool OperationGraphModel::setPortData(
    NodeId node_id,
    PortType port_type,
    PortIndex port_index,
    const QVariant& value,
    PortRole role)
{
    Q_UNUSED(node_id);
    Q_UNUSED(port_type);
    Q_UNUSED(port_index);
    Q_UNUSED(value);
    Q_UNUSED(role);

    return false;
}

} // crucible::gui
