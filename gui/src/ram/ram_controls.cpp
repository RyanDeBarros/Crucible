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

    controls_layout->addStretch();
}

} // crucible::gui
