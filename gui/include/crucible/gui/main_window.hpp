#pragma once

#include <QMainWindow>

class QDockWidget;

namespace crucible::gui
{

class ModuleInspectorPanel;
class RamPanel;

class MainWindow
    : public QMainWindow
{
    std::vector<QDockWidget*> panel_docks_;

    ModuleInspectorPanel* module_inspector_panel_ = nullptr;
    RamPanel* ram_panel_ = nullptr;

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void createPanels();
    void createMenuBar();

    void createEditMenu();

    QDockWidget* createPanelDock(
        const QString& title,
        const QString& object_name,
        QWidget* content,
        Qt::DockWidgetArea area);

    void connectPanels() const;

    void newModule();
    void newStruct();
    void editStruct();
    void newEnum();
    void editEnum();
};

} // crucible::gui
