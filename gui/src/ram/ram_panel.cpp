#include <crucible/gui/ram/ram_panel.hpp>

#include <crucible/gui/ram/ram_controls.hpp>

#include <crucible/gui/util/layout_helpers.hpp>

#include <QFrame>
#include <QVBoxLayout>

namespace crucible::gui
{

RamPanel::RamPanel(QWidget* parent)
    : QWidget(parent)
{
    createLayout();
}

void RamPanel::createLayout()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    controls_ = new RamControls(this);

    layout->addWidget(controls_);
    addSeparator(*this, *layout);
    
    layout->addStretch();

    connect(controls_, &RamControls::newModule, this, &RamPanel::createNewModule);
}

} // crucible::gui
