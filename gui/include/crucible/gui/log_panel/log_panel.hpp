#pragma once

#include <crucible/core/logger.hpp>

#include <QWidget>

class QTableView;

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
    LogControls* controls_;
    LogModel* model_;
    QTableView* table_;

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

    void setFilter(core::LogLevelMask mask);
    void refresh();
};

} // crucible::gui
