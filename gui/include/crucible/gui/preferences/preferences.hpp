#pragma once

#include <memory>

namespace crucible::gui
{

class GeneralSettings;
class LoggingSettings;

class Preferences
{
private:
    std::unique_ptr<GeneralSettings> general_;
    std::unique_ptr<LoggingSettings> logging_;

public:
    GeneralSettings& general;
    LoggingSettings& logging;

    explicit Preferences();
};

extern Preferences PREFS;

} // crucible::gui
