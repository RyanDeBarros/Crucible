#pragma once

class QIcon;

namespace crucible::gui
{

enum class Icon
{
    ClearLog,
    Filter,
    WordWrap,
};

extern const QIcon& loadIcon(Icon icon);

} // crucible::gui
