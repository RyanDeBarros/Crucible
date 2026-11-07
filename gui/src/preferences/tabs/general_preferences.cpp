#include <crucible/gui/preferences/tabs/general_preferences.hpp>

#include <QFormLayout>
#include <QSettings>

namespace crucible::gui
{

static const char* const kGeneralGroup = "general";

GeneralPreferences::GeneralPreferences(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QFormLayout(this);
}

void GeneralPreferences::load(QSettings& settings)
{
    settings.beginGroup(kGeneralGroup);
    settings.endGroup();
}

void GeneralPreferences::save(QSettings& settings) const
{
    settings.beginGroup(kGeneralGroup);
    settings.endGroup();
}

void GeneralPreferences::resetToDefaults()
{
}

} // crucible::gui
