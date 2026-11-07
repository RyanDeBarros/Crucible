#include <crucible/gui/preferences/settings/logging_settings.hpp>

#include <QSettings>

namespace crucible::gui
{

static const char* const kLoggingGroup = "logging";
static const char* const kWordWrap = "word_wrap";
static const char* const kRefreshInterval = "refresh_interval";

LoggingSettings::LoggingSettings()
    : word_wrap(false)
    , refresh_interval(250)
{
}

void LoggingSettings::load(QSettings& settings)
{
    settings.beginGroup(kLoggingGroup);

    LoggingSettings defaults;

    word_wrap = settings.value(kWordWrap, defaults.word_wrap).toBool();
    refresh_interval = settings.value(kRefreshInterval, defaults.refresh_interval).toInt();

    settings.endGroup();
}

void LoggingSettings::save(QSettings& settings) const
{
    settings.beginGroup(kLoggingGroup);

    settings.setValue(kWordWrap, word_wrap);
    settings.setValue(kRefreshInterval, refresh_interval);

    settings.endGroup();
}

void LoggingSettings::resetToDefaults()
{
    LoggingSettings defaults;
    word_wrap = defaults.word_wrap;
    refresh_interval = defaults.refresh_interval;
}

} // crucible::gui
