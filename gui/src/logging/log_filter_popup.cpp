#include <crucible/gui/logging/log_filter_popup.hpp>

#include <QCheckBox>
#include <QVBoxLayout>

namespace crucible::gui
{

LogFilterPopup::LogFilterPopup(QWidget* parent)
    : QFrame(parent, Qt::Popup)
{
    setFrameShape(QFrame::StyledPanel);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(2);

    addFilter(core::LogLevel::Debug, tr("Debug"));
    addFilter(core::LogLevel::Info, tr("Info"));
    addFilter(core::LogLevel::Warning, tr("Warning"));
    addFilter(core::LogLevel::Error, tr("Error"));

    addFilter(LogColumn::Timestamp, tr("Timestamp"));
    addFilter(LogColumn::Level, tr("Level"));
    addFilter(LogColumn::Category, tr("Category"));

    for (auto* button : level_buttons_)
        layout->addWidget(button);

    auto* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);

    layout->addWidget(line);

    for (auto* button : column_buttons_)
    {
        if (button)
            layout->addWidget(button);
    }

    updateLevelFilter();
    updateColumnFilter();
}

core::LogLevelMask LogFilterPopup::level_mask()
{
    core::LogLevelMask mask{};

    for (std::size_t i = 0; i < level_buttons_.size(); ++i)
    {
        if (level_buttons_[i]->isChecked())
            mask |= static_cast<core::LogLevel>(i);
    }

    return mask;
}

LogColumnMask LogFilterPopup::column_mask()
{
    LogColumnMask mask{};

    for (std::size_t i = 0; i < column_buttons_.size(); ++i)
    {
        if (!column_buttons_[i] || column_buttons_[i]->isChecked())
            mask |= static_cast<LogColumn>(i);
    }

    return mask;
}

void LogFilterPopup::addFilter(core::LogLevel level, const QString& label)
{
    auto* checkbox = new QCheckBox(label, this);
    checkbox->setChecked(~core::LogLevelMask(core::LogLevel::Debug) & level);
    checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    level_buttons_[static_cast<std::size_t>(level)] = checkbox;

    connect(checkbox, &QCheckBox::toggled, this, &LogFilterPopup::updateLevelFilter);
}

void LogFilterPopup::addFilter(LogColumn column, const QString& label)
{
    auto* checkbox = new QCheckBox(label, this);
    checkbox->setChecked(true);
    checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    column_buttons_[static_cast<std::size_t>(column)] = checkbox;

    connect(checkbox, &QCheckBox::toggled, this, &LogFilterPopup::updateColumnFilter);
}

void LogFilterPopup::updateLevelFilter()
{
    Q_EMIT levelFilterChanged(level_mask());
}

void LogFilterPopup::updateColumnFilter()
{
    Q_EMIT columnFilterChanged(column_mask());
}

} // crucible::gui
