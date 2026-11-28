#pragma once

#include <ostream>

class QIcon;

namespace crucible::gui
{

enum class Icon
{
    Apply,
    BackArrow,
    Cancel,
    ClearLog,
    Close,
    Delete,
    DeleteAll,
    DragHandle,
    Edit,
    EditEnum,
    EditStruct,
    ExpandDown,
    ExpandRight,
    Filter,
    NewModule,
    Plus,
    WordWrap,
};

extern std::ostream& operator<<(std::ostream& os, Icon icon);

extern const QIcon& loadIcon(Icon icon);

} // crucible::gui
