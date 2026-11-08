#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/preferences/settings/general_settings.hpp>
#include <crucible/gui/preferences/settings/logging_settings.hpp>

#include <QSettings>
#include <QStandardPaths>

namespace crucible::gui
{

Preferences PREFS;

Preferences::Preferences()
    : general_(std::make_unique<GeneralSettings>())
    , logging_(std::make_unique<LoggingSettings>())
    , general(*general_)
    , logging(*logging_)
{
}

void Preferences::loadSettings()
{
    auto settings = getSettings();

    general.load(settings);
    logging.load(settings);
}

QSettings Preferences::getSettings() const
{
    return QSettings(
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/preferences.ini",
        QSettings::IniFormat);
}

} // crucible::gui
