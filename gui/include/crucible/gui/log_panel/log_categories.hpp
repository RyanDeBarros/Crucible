#pragma once

#include <crucible/core/log_categories.hpp>

#include <QColor>

namespace crucible::gui
{

extern const core::LogCategory kApp;
extern const core::LogCategory kLog;
extern const core::LogCategory kOpGraph;

extern QColor logCategoryColor(const core::LogCategory& category);

} // crucible::gui
