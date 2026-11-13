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
    ModuleInspectorPanel(QWidget* parent = nullptr);

signals:
    void newStruct();
    void editStruct();
    void newEnum();
    void editEnum();

public:
    void newModule();

private:
    void createLayout();
};

} // crucible::gui
