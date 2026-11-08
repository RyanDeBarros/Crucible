#include <crucible/gui/preferences/settings/logging_settings.hpp>

#include <QSettings>

namespace crucible::gui
{

static const char* const kLoggingGroup = "logging";
static const char* const kRefreshInterval = "refresh_interval";

LoggingSettings::LoggingSettings()
    : refresh_interval(250)
{
}

void LoggingSettings::load(QSettings& settings)
{
    settings.beginGroup(kLoggingGroup);

    LoggingSettings defaults;

    refresh_interval.set(settings.value(kRefreshInterval, *defaults.refresh_interval).toInt());

    settings.endGroup();
}

void LoggingSettings::save(QSettings& settings) const
{
    settings.beginGroup(kLoggingGroup);

    settings.setValue(kRefreshInterval, *refresh_interval);

    settings.endGroup();
}

void LoggingSettings::resetToDefaults()
{
    LoggingSettings defaults;

    refresh_interval.set(*defaults.refresh_interval);
}

} // crucible::gui
