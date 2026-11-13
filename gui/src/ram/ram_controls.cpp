#include <crucible/gui/ram/ram_controls.hpp>

#include <crucible/gui/util/icons.hpp>

#include <QHBoxLayout>
#include <QToolButton>

namespace crucible::gui
{

RamControls::RamControls(QWidget* parent)
    : QWidget(parent)
{
    auto* controls_layout = new QHBoxLayout(this);
    controls_layout->setContentsMargins(4, 2, 4, 2);

    createNewModuleButton();

    controls_layout->addWidget(new_module_button_);
    controls_layout->addStretch();
}

void RamControls::createNewModuleButton()
{
    new_module_button_ = new QToolButton(this);
    new_module_button_->setIcon(loadIcon(Icon::NewModule));
    new_module_button_->setToolTip(tr("New module"));
    connect(new_module_button_, &QToolButton::clicked, this, &RamControls::newModule);
}

} // crucible::gui
