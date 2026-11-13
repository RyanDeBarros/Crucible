#pragma once

class QString;

namespace crucible::gui
{

enum class StyleSheet
{
    MainWindow,
    Separator,
    ToggleIcon,
};

extern const QString& loadStyleSheet(StyleSheet stylesheet);

} // crucible::gui
