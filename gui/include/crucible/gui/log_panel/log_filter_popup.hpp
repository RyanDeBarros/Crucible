#pragma once

#include <crucible/core/logging/log_level.hpp>

#include <QFrame>

class QCheckBox;

namespace crucible::gui
{

class LogFilterPopup
    : public QFrame
{
    Q_OBJECT;

private:
    std::array<QCheckBox*, static_cast<std::size_t>(core::LogLevel::_Count)> filter_buttons_{};

public:
    explicit LogFilterPopup(core::LogLevelMask initialFilter, QWidget* parent = nullptr);

    core::LogLevelMask mask();

signals:
    void filterChanged(core::LogLevelMask mask);

private:
    void addFilter(core::LogLevel level, const QString& label, core::LogLevelMask initialFilter);
    void updateFilter();
};

} // crucible::gui
