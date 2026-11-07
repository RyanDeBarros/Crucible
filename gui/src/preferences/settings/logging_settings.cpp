#include <crucible/gui/preferences/settings/logging_settings.hpp>

#include <QSettings>

namespace crucible::gui
{

static const char* const kLoggingGroup = "logging";
static const char* const kWordWrap = "word_wrap";
static const char* const kRefreshInterval = "refresh_interval";

LoggingSettings::LoggingSettings()
    : word_wrap_(false)
    , refresh_interval_(250)
{
}

void LoggingSettings::load(QSettings& settings)
{
    settings.beginGroup(kLoggingGroup);

    LoggingSettings defaults;

    setWordWrap(settings.value(kWordWrap, defaults.wordWrap()).toBool());
    setRefreshInterval(settings.value(kRefreshInterval, defaults.refreshInterval()).toInt());

    settings.endGroup();
}

void LoggingSettings::save(QSettings& settings) const
{
    settings.beginGroup(kLoggingGroup);

    settings.setValue(kWordWrap, wordWrap());
    settings.setValue(kRefreshInterval, refreshInterval());

    settings.endGroup();
}

void LoggingSettings::resetToDefaults()
{
    LoggingSettings defaults;

    setWordWrap(defaults.wordWrap());
    setRefreshInterval(defaults.refreshInterval());
}

bool LoggingSettings::wordWrap() const
{
    return word_wrap_;
}

int LoggingSettings::refreshInterval() const
{
    return refresh_interval_;
}

void LoggingSettings::setWordWrap(bool word_wrap)
{
    word_wrap_ = word_wrap;
    modified(WordWrap);
}

void LoggingSettings::setRefreshInterval(int refresh_interval)
{
    refresh_interval_ = refresh_interval;
    modified(RefreshInterval);
}

} // crucible::gui
