#pragma once

#include <crucible/core/logger.hpp>

#include <QTableView>
#include <QWidget>

namespace crucible::core
{

class Logger;

} // crucible::core

namespace crucible::gui
{

class LogModel;

class LogPanel
    : public QWidget
{
    Q_OBJECT;

private:
    LogModel* model_;
    QTableView* table_;

    QToolButton* filter_button_ = nullptr;
    QMenu* filter_menu_;
    std::array<QAction*, static_cast<size_t>(core::LogLevel::_Count)> filter_actions_{};

public:
    explicit LogPanel(const core::Logger& logger, QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

public:
    void setWordWrap(bool on);

private:
    void createFilterButton();
    void createTable();
    void createLayout();
    void initPoll();

    void refresh();
    void updateFilter();
};

} // crucible::gui
