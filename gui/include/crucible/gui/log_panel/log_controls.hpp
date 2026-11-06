#pragma once

#include <crucible/core/logger.hpp>

#include <QHBoxLayout>
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
    QToolButton* clear_button_ = nullptr;
    
    LogFilterPopup* filter_popup_ = nullptr;

public:
    explicit LogControls(QWidget* parent = nullptr);

signals:
    void filterChanged(core::LogLevelMask mask);
    void clearRequested();

private:
    void createFilterButton();
    void toggleFilterPopup();

    void createClearButton();
};

} // crucible::gui
