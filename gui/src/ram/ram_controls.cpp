#include <crucible/gui/ram/ram_controls.hpp>

#include <crucible/gui/util/icons.hpp>
#include <crucible/gui/util/layout_helpers.hpp>

#include <QHBoxLayout>
#include <QToolButton>

namespace crucible::gui
{

RamControls::RamControls(QWidget* parent)
    : QWidget(parent)
{
    auto* controls_layout = new QHBoxLayout(this);
    controls_layout->setContentsMargins(4, 2, 4, 2);

    auto* new_module_button = new QToolButton(this);
    new_module_button->setIcon(loadIcon(Icon::NewModule));
    new_module_button->setToolTip(tr("New module"));
    connect(new_module_button, &QToolButton::clicked, this, &RamControls::newModule);
    controls_layout->addWidget(new_module_button);

    addSeparator(*this, *controls_layout);

    auto* edit_struct_button = new QToolButton(this);
    edit_struct_button->setIcon(loadIcon(Icon::EditStruct));
    edit_struct_button->setToolTip(tr("Edit structs"));
    connect(edit_struct_button, &QToolButton::clicked, this, &RamControls::editStructs);
    controls_layout->addWidget(edit_struct_button);

    auto* edit_enum_button = new QToolButton(this);
    edit_enum_button->setIcon(loadIcon(Icon::EditEnum));
    edit_enum_button->setToolTip(tr("Edit enums"));
    connect(edit_enum_button, &QToolButton::clicked, this, &RamControls::editEnums);
    controls_layout->addWidget(edit_enum_button);

    controls_layout->addStretch();
}

} // crucible::gui
