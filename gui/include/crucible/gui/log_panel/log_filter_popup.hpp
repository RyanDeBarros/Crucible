#pragma once

#include <crucible/core/logger.hpp>

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
    explicit LogFilterPopup(QWidget* parent = nullptr);

signals:
    void filterChanged(core::LogLevelMask mask);

private:
    void addFilter(core::LogLevel level, const QString& label);
    void updateFilter();
};

} // crucible::gui
