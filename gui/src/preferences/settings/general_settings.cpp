#include <crucible/gui/preferences/settings/general_settings.hpp>

#include <QSettings>

namespace crucible::gui
{

static const char* const kGeneralGroup = "general";

GeneralSettings::GeneralSettings()
{
}

void GeneralSettings::load(QSettings& settings)
{
    settings.beginGroup(kGeneralGroup);

    GeneralSettings defaults;

    settings.endGroup();
}

void GeneralSettings::save(QSettings& settings) const
{
    settings.beginGroup(kGeneralGroup);

    settings.endGroup();
}

void GeneralSettings::resetToDefaults()
{
    GeneralSettings defaults;
}

} // crucible::gui
