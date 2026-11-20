#include <crucible/gui/ram/ram_panel.hpp>

#include <crucible/gui/dialogs/edit_enums_dialog.hpp>

#include <crucible/gui/ram/ram_controls.hpp>

#include <crucible/gui/logging/logger_include.hpp>

#include <crucible/gui/util/layout_helpers.hpp>

#include <QFrame>
#include <QVBoxLayout>

namespace crucible::gui
{

RamPanel::RamPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    controls_ = new RamControls(this);

    layout->addWidget(controls_);
    addSeparator(*this, *layout);

    layout->addStretch();

    connect(controls_, &RamControls::newModule, this, &RamPanel::newModule);
    connect(controls_, &RamControls::editStructs, this, &RamPanel::editStructs);
    connect(controls_, &RamControls::editEnums, this, &RamPanel::editEnums);
}

void RamPanel::newModule()
{
    // TODO
    LOG.info(kModules) << "New module";
}

void RamPanel::editStructs()
{
    // TODO
    LOG.info(kModules) << "Edit structs!";
}

void RamPanel::editEnums()
{
    EditEnumsDialog dialog(this);
    dialog.exec();
}

} // crucible::gui
