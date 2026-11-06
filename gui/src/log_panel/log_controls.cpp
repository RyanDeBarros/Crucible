#include <crucible/gui/log_panel/log_controls.hpp>

#include <crucible/gui/log_panel/log_filter_popup.hpp>

#include <QHBoxLayout>
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
    connect(filter_button_, &QToolButton::clicked, this, &LogControls::toggleFilterPopup);

    filter_popup_ = new LogFilterPopup(this);
    connect(filter_popup_, &LogFilterPopup::filterChanged, this, &LogControls::filterChanged);
}

void LogControls::toggleFilterPopup()
{
    if (filter_popup_->isVisible())
    {
        filter_popup_->hide();
        return;
    }

    filter_popup_->adjustSize();

    const QPoint position = filter_button_->mapToGlobal(QPoint(0, filter_button_->height()));

    filter_popup_->move(position);
    filter_popup_->show();
}

void LogControls::createClearButton()
{
    clear_button_ = new QToolButton(this);
    clear_button_->setIcon(QIcon(":/icons/clear_log"));
    clear_button_->setToolTip(tr("Clear log"));

    connect(clear_button_, &QToolButton::clicked, this, &LogControls::clearRequested);
}

} // crucible::gui
