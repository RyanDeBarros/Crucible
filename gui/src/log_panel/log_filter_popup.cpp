#include <crucible/gui/log_panel/log_filter_popup.hpp>

#include <QCheckBox>
#include <QVBoxLayout>

namespace crucible::gui
{

LogFilterPopup::LogFilterPopup(core::LogLevelMask initialFilter, QWidget* parent)
    : QFrame(parent, Qt::Popup)
{
    setFrameShape(QFrame::StyledPanel);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(2);

    addFilter(core::LogLevel::Debug, tr("Debug"), initialFilter);
    addFilter(core::LogLevel::Info, tr("Info"), initialFilter);
    addFilter(core::LogLevel::Warning, tr("Warning"), initialFilter);
    addFilter(core::LogLevel::Error, tr("Error"), initialFilter);

    for (auto* button : filter_buttons_)
        layout->addWidget(button);

    updateFilter();
}

core::LogLevelMask LogFilterPopup::mask()
{
    core::LogLevelMask mask = 0;

    for (std::size_t i = 0; i < filter_buttons_.size(); ++i)
    {
        if (filter_buttons_[i]->isChecked())
            mask |= core::logLevelMask(static_cast<core::LogLevel>(i));
    }

    return mask;
}

void LogFilterPopup::addFilter(core::LogLevel level, const QString& label, core::LogLevelMask initialFilter)
{
    auto* checkbox = new QCheckBox(label, this);
    checkbox->setChecked(core::logLevelMask(level) & initialFilter);
    checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    filter_buttons_[static_cast<std::size_t>(level)] = checkbox;

    connect(checkbox, &QCheckBox::toggled, this, &LogFilterPopup::updateFilter);
}

void LogFilterPopup::updateFilter()
{
    emit filterChanged(mask());
}

} // crucible::gui
