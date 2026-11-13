#pragma once

#include <crucible/core/utility/utility.hpp>

#include <QWidget>

namespace crucible::gui
{

class ModuleInspectorPanel
    : public QWidget
{
    Q_OBJECT;

public:
    ModuleInspectorPanel(QWidget* parent = nullptr);

    void newModule();
};

inline core::SafePointer<ModuleInspectorPanel> gModuleInspectorPanel;

} // crucible::gui
