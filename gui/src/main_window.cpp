#include <crucible/gui/main_window.hpp>

#include <crucible/gui/logging/logger_include.hpp>
#include <crucible/gui/logging/log_panel.hpp>

#include <crucible/gui/module_inspector/module_inspector_panel.hpp>

#include <crucible/gui/op_graph/op_graph_panel.hpp>

#include <crucible/gui/preferences/preferences_dialog.hpp>

#include <crucible/gui/ram/ram_panel.hpp>

#include <QDockWidget>
#include <QMenuBar>
#include <QScreen>
#include <QStatusBar>

namespace crucible::gui
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Crucible");
    resize(1920, 1080);
    move(screen()->geometry().center() - frameGeometry().center());

    setDockNestingEnabled(true);
    setDockOptions(AnimatedDocks | AllowTabbedDocks | AllowNestedDocks);

    createPanels();
    createMenuBar();
    connectPanels();

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createPanels()
{
    LOG.debug(kApp) << "Creating panels...";

    ram_panel_ = new RamPanel();
    auto* ram_dock = createPanelDock(
        tr("RAM"),
        QStringLiteral("ram_dock"),
        ram_panel_,
        Qt::LeftDockWidgetArea);

    auto* log_dock = createPanelDock(
        tr("Log"),
        QStringLiteral("log_dock"),
        new LogPanel(LOG),
        Qt::LeftDockWidgetArea);

    splitDockWidget(ram_dock, log_dock, Qt::Vertical);

    auto* telemetry_dock = createPanelDock(
        tr("Telemetry"),
        QStringLiteral("telemetry_dock"),
        new QWidget(),
        Qt::LeftDockWidgetArea);

    auto* op_explorer_dock = createPanelDock(
        tr("Operation Explorer"),
        QStringLiteral("op_explorer_dock"),
        new QWidget(),
        Qt::LeftDockWidgetArea);

    tabifyDockWidget(ram_dock, telemetry_dock);
    tabifyDockWidget(ram_dock, op_explorer_dock);

    ram_dock->raise();

    auto* op_graph_dock = createPanelDock(
        tr("Operation Graph"),
        QStringLiteral("op_graph_dock"),
        new OpGraphPanel(),
        Qt::RightDockWidgetArea);

    auto* op_output_dock = createPanelDock(
        tr("Operation Output"),
        QStringLiteral("op_output_dock"),
        new QWidget(),
        Qt::RightDockWidgetArea);

    splitDockWidget(op_graph_dock, op_output_dock, Qt::Vertical);

    module_inspector_panel_ = new ModuleInspectorPanel();
    auto* module_inspector_dock = createPanelDock(
        tr("Module Inspector"),
        QStringLiteral("module_inspector_dock"),
        module_inspector_panel_,
        Qt::RightDockWidgetArea);

    tabifyDockWidget(op_graph_dock, module_inspector_dock);

    op_graph_dock->raise();

    resizeDocks({ ram_dock, op_graph_dock }, { 480, 1440 }, Qt::Horizontal);
    resizeDocks({ ram_dock, log_dock }, { 700, 300 }, Qt::Vertical);
    resizeDocks({ op_graph_dock, op_output_dock }, { 750, 250 }, Qt::Vertical);

    LOG.debug(kApp) << "...panels created";


    // TODO remove debug
    module_inspector_dock->raise();
}

void MainWindow::createMenuBar()
{
    createEditMenu();

    auto* window_menu = menuBar()->addMenu(tr("Window"));

    for (auto* dock : panel_docks_)
        window_menu->addAction(dock->toggleViewAction());
}

void MainWindow::createEditMenu()
{
    auto* edit_menu = menuBar()->addMenu(tr("Edit"));

    auto* new_module_action = edit_menu->addAction(
        tr("New Module"),
        QKeySequence(Qt::ControlModifier | Qt::Key_M));

    connect(new_module_action, &QAction::triggered, this, &MainWindow::createNewModule);

    edit_menu->addSeparator();

    auto* preferences_action = edit_menu->addAction(
        tr("Preferences..."),
        QKeySequence(Qt::ControlModifier | Qt::Key_Comma));
    
    connect(preferences_action, &QAction::triggered, this, [this]() {
        PreferencesDialog dialog(this);
        dialog.exec();
    });
}

QDockWidget* MainWindow::createPanelDock(
    const QString& title,
    const QString& object_name,
    QWidget* content,
    Qt::DockWidgetArea area)
{
    auto* dock = new QDockWidget(title, this);

    dock->setObjectName(object_name);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setFeatures(
        QDockWidget::DockWidgetClosable |
        QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable);

    dock->setWidget(content);

    addDockWidget(area, dock);
    panel_docks_.push_back(dock);

    return dock;
}

void MainWindow::connectPanels() const
{
    connect(ram_panel_, &RamPanel::createNewModule, this, &MainWindow::createNewModule);
}

void MainWindow::createNewModule()
{
    module_inspector_panel_->newModule();
}

} // crucible::gui
