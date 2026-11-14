#pragma once

#include <QWidget>

namespace crucible::gui
{

class ModuleInspectorControls
    : public QWidget
{
    Q_OBJECT;

public:
    explicit ModuleInspectorControls(QWidget* parent = nullptr);

signals:
    void newModule();
    void newStruct();
    void editStruct();
    void newEnum();
    void editEnum();
};

} // crucible::gui
