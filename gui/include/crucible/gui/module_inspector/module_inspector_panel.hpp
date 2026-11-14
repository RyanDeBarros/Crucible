#pragma once

#include <crucible/core/utility/utility.hpp>

#include <QWidget>

namespace crucible::gui
{

class ModuleInspectorControls;

class ModuleInspectorPanel
    : public QWidget
{
    Q_OBJECT;

private:
    ModuleInspectorControls* controls_ = nullptr;

public:
    explicit ModuleInspectorPanel(QWidget* parent = nullptr);

public:
    void newModule();
    void newStruct();
    void editStruct();
    void newEnum();
    void editEnum();
};

} // crucible::gui
