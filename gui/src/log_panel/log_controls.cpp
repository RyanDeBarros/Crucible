#include <crucible/gui/log_panel/log_controls.hpp>

#include <QMenu>
#include <QToolButton>

namespace crucible::gui
{

LogControls::LogControls(QWidget* parent)
    : QWidget(parent)
{
    auto* controls_layout = new QHBoxLayout(this);
    controls_layout->setContentsMargins(4, 2, 4, 2);

    createFilterButton();
    createClearButton();

    controls_layout->addWidget(filter_button_);
    controls_layout->addWidget(clear_button_);
    controls_layout->addStretch();
}

void LogControls::createFilterButton()
{
    filter_button_ = new QToolButton(this);
    filter_button_->setIcon(QIcon(":/icons/filter"));
    filter_button_->setToolTip(tr("Filter log levels"));
    filter_button_->setPopupMode(QToolButton::InstantPopup);

    filter_menu_ = new QMenu(filter_button_);

    addFilter(core::LogLevel::Debug, tr("Debug"));
    addFilter(core::LogLevel::Info, tr("Info"));
    addFilter(core::LogLevel::Warning, tr("Warning"));
    addFilter(core::LogLevel::Error, tr("Error"));

    filter_button_->setMenu(filter_menu_);
}

void LogControls::addFilter(core::LogLevel level, const QString& label)
{
    auto* action = filter_menu_->addAction(label);
    action->setCheckable(true);
    action->setChecked(true);

    filter_actions_[static_cast<size_t>(level)] = action;

    connect(action, &QAction::toggled, this, &LogControls::updateFilter);
}

void LogControls::updateFilter()
{
    core::LogLevelMask mask = 0;

    for (size_t i = 0; i < filter_actions_.size(); ++i)
    {
        if (filter_actions_[i]->isChecked())
            mask |= core::logLevelMask(static_cast<core::LogLevel>(i));
    }

    filterChanged(mask);
}

void LogControls::createClearButton()
{
    clear_button_ = new QToolButton(this);
    clear_button_->setIcon(QIcon(":/icons/clear_log"));
    clear_button_->setToolTip(tr("Clear log"));

    connect(clear_button_, &QToolButton::clicked, this, &LogControls::clearRequested);
}

} // crucible::gui
