#pragma once

#include <QWidget>

namespace crucible::gui
{

class OpGraphModel;
class OpGraphScene;
class OpGraphView;

class OpGraphPanel
    : public QWidget
{
    Q_OBJECT;

private:
    std::unique_ptr<OpGraphModel> model_;
    std::unique_ptr<OpGraphScene> scene_;

    OpGraphView* view_ = nullptr;

public:
    explicit OpGraphPanel(QWidget* parent = nullptr);
    ~OpGraphPanel() override;

    OpGraphModel& model();
    OpGraphScene& scene();
    OpGraphView& view();
};


} // crucible::gui
