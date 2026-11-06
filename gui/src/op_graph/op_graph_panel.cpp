#include <crucible/gui/op_graph/op_graph_panel.hpp>

#include <crucible/gui/op_graph/op_graph_model.hpp>
#include <crucible/gui/op_graph/op_graph_scene.hpp>
#include <crucible/gui/op_graph/op_graph_view.hpp>

#include <crucible/gui/log_panel/log_categories.hpp>

#include <crucible/core/logger.hpp>

#include <QVBoxLayout>

namespace crucible::gui
{

OpGraphPanel::OpGraphPanel(QWidget* parent)
    : QWidget(parent)
    , model_(std::make_unique<OpGraphModel>())
    , scene_(std::make_unique<OpGraphScene>(*model_))
{
    LOG.debug(kOpGraph) << "Initializing operation graph panel...";

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    view_ = new OpGraphView(*scene_, *model_);

    layout->addWidget(view_);
}

OpGraphPanel::~OpGraphPanel() = default;

OpGraphModel& OpGraphPanel::model()
{
    return *model_;
}

OpGraphScene& OpGraphPanel::scene()
{
    return *scene_;
}

OpGraphView& OpGraphPanel::view()
{
    return *view_;
}

} // crucible::gui
