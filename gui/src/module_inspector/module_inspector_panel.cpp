#include <crucible/gui/module_inspector/module_inspector_panel.hpp>

#include <crucible/gui/logging/logger_include.hpp>

namespace crucible::gui
{

void ModuleInspectorPanel::newModule()
{
    LOG.info(kModules) << "New module";
}

} // crucible::gui
