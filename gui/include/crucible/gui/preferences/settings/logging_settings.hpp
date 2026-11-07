#pragma once

#include <crucible/gui/util/signal_property.hpp>

class QSettings;

namespace crucible::gui
{

struct LoggingSettings
{
    SignalProperty<bool> word_wrap;
    SignalProperty<int> refresh_interval;

    explicit LoggingSettings();

    void load(QSettings& settings);
    void save(QSettings& settings) const;
    void resetToDefaults();
};

} // crucible::gui
