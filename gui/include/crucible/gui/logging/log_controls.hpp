#pragma once

#include <crucible/core/logging/log_level.hpp>

#include <crucible/gui/logging/log_column.hpp>

#include <QWidget>

namespace crucible::gui
{

class LogFilterPopup;

class LogControls
    : public QWidget
{
    Q_OBJECT;

private:
    QToolButton* filter_button_ = nullptr;
    QToolButton* word_wrap_button_ = nullptr;
    QToolButton* clear_button_ = nullptr;
    
    LogFilterPopup* filter_popup_ = nullptr;

public:
    explicit LogControls(QWidget* parent = nullptr);

    core::LogLevelMask level_mask() const;
    LogColumnMask column_mask() const;
    bool wordWrap() const;

signals:
    void levelFilterChanged(core::LogLevelMask mask);
    void columnFilterChanged(LogColumnMask mask);
    void wordWrapToggled(bool on);
    void clearRequested();

private:
    void createFilterButton();
    void createWordWrapButton();
    void createClearButton();

    void toggleFilterPopup();
};

} // crucible::gui
