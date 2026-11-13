#pragma once

#include <ostream>

class QIcon;

namespace crucible::gui
{

enum class Icon
{
    ClearLog,
    EditEnum,
    EditStruct,
    Filter,
    NewEnum,
    NewModule,
    NewStruct,
    WordWrap,
};

extern std::ostream& operator<<(std::ostream& os, Icon icon);

extern const QIcon& loadIcon(Icon icon);

} // crucible::gui
