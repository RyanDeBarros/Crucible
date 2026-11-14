#include <crucible/gui/module_inspector/module_inspector_panel.hpp>

#include <crucible/gui/module_inspector/module_inspector_controls.hpp>
#include <crucible/gui/module_inspector/edit_enums_dialog.hpp>

#include <crucible/gui/logging/logger_include.hpp>

#include <crucible/gui/util/layout_helpers.hpp>

#include <QVBoxLayout>

namespace crucible::gui
{

ModuleInspectorPanel::ModuleInspectorPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    controls_ = new ModuleInspectorControls(this);

    layout->addWidget(controls_);
    addSeparator(*this, *layout);

    layout->addStretch();

    connect(controls_, &ModuleInspectorControls::newModule, this, &ModuleInspectorPanel::newModule);
    connect(controls_, &ModuleInspectorControls::editStructs, this, &ModuleInspectorPanel::editStructs);
    connect(controls_, &ModuleInspectorControls::editEnums, this, &ModuleInspectorPanel::editEnums);
}

void ModuleInspectorPanel::newModule()
{
    LOG.info(kModules) << "New module";
}

void ModuleInspectorPanel::editStructs()
{
    LOG.info(kModules) << "Edit structs!";
}

void ModuleInspectorPanel::editEnums()
{
    EditEnumsDialog dialog(this);
    dialog.exec();
}

} // crucible::gui
