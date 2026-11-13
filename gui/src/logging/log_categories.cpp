#include <crucible/gui/logging/log_categories.hpp>

#include <utility>

namespace crucible::gui
{

const core::LogCategory kApp = { "App" };
const core::LogCategory kLog = { "Log" };
const core::LogCategory kModules = { "Modules" };
const core::LogCategory kOpGraph = { "Op Graph" };

QColor logCategoryColor(const core::LogCategory& category)
{
    if (category == kApp)
        return QColor(0x72, 0xB7, 0xB0);
    else if (category == core::kCore)
        return QColor(0x80, 0xA9, 0xD6);
    else if (category == core::kIO)
        return QColor(0xD5, 0xA6, 0x6F);
    else if (category == kLog)
        return QColor(0xA0, 0xA0, 0xA0);
    else if (category == kModules)
        return QColor(0xA3, 0xB8, 0x8C);
    else if (category == kOpGraph)
        return QColor(0xB3, 0x9D, 0xDB);
    else
        return {};
}

} // crucible::gui
