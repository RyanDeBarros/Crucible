#pragma once

#include <crucible/core/logger.hpp>

#include <QHBoxLayout>
#include <QWidget>

namespace crucible::gui
{

class LogControls
    : public QWidget
{
    Q_OBJECT;

private:
    QToolButton* filter_button_ = nullptr;
    QToolButton* clear_button_ = nullptr;
    
    QMenu* filter_menu_;
    std::array<QAction*, static_cast<size_t>(core::LogLevel::_Count)> filter_actions_{};

public:
    explicit LogControls(QWidget* parent = nullptr);

signals:
    void filterChanged(core::LogLevelMask mask);
    void clearRequested();

private:
    void createFilterButton();
    void addFilter(core::LogLevel level, const QString& label);
    void updateFilter();

    void createClearButton();
};

} // crucible::gui
