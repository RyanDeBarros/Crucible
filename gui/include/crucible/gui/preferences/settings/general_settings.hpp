#pragma once

class QSettings;

namespace crucible::gui
{

struct GeneralSettings
{
    explicit GeneralSettings();

    void load(QSettings& settings);
    void save(QSettings& settings) const;
    void resetToDefaults();
};

} // crucible::gui
