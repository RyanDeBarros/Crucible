#pragma once

#include <crucible/core/logging/logger.hpp>

#include <crucible/gui/log_panel/log_column.hpp>

#include <QWidget>

class QTableView;
class QTimer;

namespace crucible::core
{

class Logger;

} // crucible::core

namespace crucible::gui
{

class LogControls;
class LogModel;

class LogPanel
    : public QWidget
{
    Q_OBJECT;

private:
    LogControls* controls_ = nullptr;
    LogModel* model_ = nullptr;
    QTableView* table_ = nullptr;
    QTimer* poll_timer_ = nullptr;

public:
    explicit LogPanel(core::Logger& logger, QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    void setWordWrap(bool on);

private:
    void createLayout();
    void createTable();
    void initPoll();

    void refresh();

    void setLevelFilter(core::LogLevelMask mask);
    void setColumnFilter(LogColumnMask mask);
    void requestClearLog();
};

} // crucible::gui
