#include <crucible/gui/preferences/tabs/logging_preferences.hpp>

#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/preferences/settings/logging_settings.hpp>

#include <crucible/gui/util/block_signals.hpp>

#include <QFormLayout>
#include <QSpinBox>

namespace crucible::gui
{

LoggingPreferences::LoggingPreferences(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QFormLayout(this);

    refresh_interval_ = new QSpinBox(this);
    refresh_interval_->setRange(50, 5000);
    refresh_interval_->setSingleStep(50);
    refresh_interval_->setSuffix(tr(" ms"));
    refresh_interval_->setValue(*PREFS.logging.refresh_interval);
    connect(refresh_interval_, &QSpinBox::valueChanged, this, &LoggingPreferences::modified);
    layout->addRow(tr("Refresh interval"), refresh_interval_);
}

void LoggingPreferences::save(QSettings& settings) const
{
    PREFS.logging.refresh_interval.set(refresh_interval_->value());

    PREFS.logging.save(settings);
}

void LoggingPreferences::resetToDefaults()
{
    PREFS.logging.resetToDefaults();

    refresh_interval_->setValue(*PREFS.logging.refresh_interval);
}

} // crucible::gui
