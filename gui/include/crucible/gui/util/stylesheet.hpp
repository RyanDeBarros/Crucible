#pragma once

class QString;

namespace crucible::gui
{

enum class Stylesheet
{
    Dark,
    ToggleIcon,
};

extern const QString& loadStylesheet(Stylesheet stylesheet);

} // crucible::gui
