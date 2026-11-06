#pragma once

#include <crucible/core/logging/log_level.hpp>

#include <crucible/gui/log_panel/log_column.hpp>

#include <QFrame>

class QCheckBox;

namespace crucible::gui
{

class LogFilterPopup
    : public QFrame
{
    Q_OBJECT;

private:
    std::array<QCheckBox*, static_cast<std::size_t>(core::LogLevel::_Count)> level_buttons_{};
    std::array<QCheckBox*, static_cast<std::size_t>(LogColumn::_Count)> column_buttons_{};

public:
    explicit LogFilterPopup(QWidget* parent = nullptr);

    core::LogLevelMask level_mask();
    LogColumnMask column_mask();

signals:
    void levelFilterChanged(core::LogLevelMask mask);
    void columnFilterChanged(LogColumnMask mask);

private:
    void addFilter(core::LogLevel level, const QString& label);
    void addFilter(LogColumn column, const QString& label);
    void updateLevelFilter();
    void updateColumnFilter();
};

} // crucible::gui
