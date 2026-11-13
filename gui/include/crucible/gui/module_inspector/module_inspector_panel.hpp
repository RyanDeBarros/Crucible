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
    void newModule();
};

} // crucible::gui
