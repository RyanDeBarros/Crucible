#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/preferences/settings/general_settings.hpp>
#include <crucible/gui/preferences/settings/logging_settings.hpp>

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

} // crucible::gui
