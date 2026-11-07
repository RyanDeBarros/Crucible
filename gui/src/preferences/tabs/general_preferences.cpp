#include <crucible/gui/preferences/tabs/general_preferences.hpp>

#include <crucible/gui/preferences/preferences.hpp>

#include <crucible/gui/preferences/settings/general_settings.hpp>

#include <QFormLayout>

namespace crucible::gui
{

GeneralPreferences::GeneralPreferences(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QFormLayout(this);
}

void GeneralPreferences::load(QSettings& settings)
{
    PREFS.general.load(settings);
}

void GeneralPreferences::save(QSettings& settings) const
{
    PREFS.general.save(settings);
}

void GeneralPreferences::resetToDefaults()
{
    PREFS.general.resetToDefaults();
}

} // crucible::gui
