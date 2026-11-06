#include <crucible/gui/main_window.hpp>

#include <crucible/gui/log_panel/log_panel.hpp>
#include <crucible/gui/op_graph/op_graph_panel.hpp>

#include <crucible/core/logger.hpp>

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

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createPanels()
{
    auto* ram_dock = createPanelDock(
        tr("RAM"),
        QStringLiteral("ram_dock"),
        new QWidget(),
        Qt::LeftDockWidgetArea);

    auto* log_panel = new LogPanel(LOG);
    auto* log_dock = createPanelDock(
        tr("Log"),
        QStringLiteral("log_dock"),
        log_panel,
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

    auto* op_graph_panel = new OpGraphPanel();
    auto* op_graph_dock = createPanelDock(
        tr("Operation Graph"),
        QStringLiteral("op_graph_dock"),
        op_graph_panel,
        Qt::RightDockWidgetArea);

    auto* op_output_dock = createPanelDock(
        tr("Operation Output"),
        QStringLiteral("op_output_dock"),
        new QWidget(),
        Qt::RightDockWidgetArea);

    splitDockWidget(op_graph_dock, op_output_dock, Qt::Vertical);

    resizeDocks({ ram_dock, op_graph_dock }, { 480, 1440 }, Qt::Horizontal);
    resizeDocks({ ram_dock, log_dock }, { 700, 300 }, Qt::Vertical);
    resizeDocks({ op_graph_dock, op_output_dock }, { 750, 250 }, Qt::Vertical);
}

void MainWindow::createMenuBar()
{
    auto* window_menu = menuBar()->addMenu(tr("Window"));

    for (auto* dock : panel_docks_)
        window_menu->addAction(dock->toggleViewAction());
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

} // crucible::gui
