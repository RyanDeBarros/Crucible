#pragma once

#include <QWidget>

class QToolButton;

namespace crucible::gui
{

class ModuleInspectorControls
    : public QWidget
{
    Q_OBJECT;

private:
    QToolButton* new_module_button_ = nullptr;

public:
    ModuleInspectorControls(QWidget* parent = nullptr);

signals:
    void newModule();

private:
    void createNewModuleButton();
};

} // crucible::gui
