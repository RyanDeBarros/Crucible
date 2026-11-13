#include <crucible/gui/module_inspector/module_inspector_panel.hpp>

#include <crucible/gui/module_inspector/module_inspector_controls.hpp>

#include <crucible/gui/logging/logger_include.hpp>

#include <crucible/gui/util/layout_helpers.hpp>

#include <QVBoxLayout>

namespace crucible::gui
{

ModuleInspectorPanel::ModuleInspectorPanel(QWidget* parent)
    : QWidget(parent)
{
    createLayout();
}

void ModuleInspectorPanel::newModule()
{
    LOG.info(kModules) << "New module";
}

void ModuleInspectorPanel::createLayout()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    controls_ = new ModuleInspectorControls(this);

    layout->addWidget(controls_);
    addSeparator(*this, *layout);

    layout->addStretch();

    connect(controls_, &ModuleInspectorControls::newModule, this, &ModuleInspectorPanel::newModule);
}

} // crucible::gui
