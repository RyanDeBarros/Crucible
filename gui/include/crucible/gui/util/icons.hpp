#pragma once

#include <ostream>

class QIcon;

namespace crucible::gui
{

enum class Icon
{
    BackArrow,
    ClearLog,
    Close,
    Delete,
    DeleteAll,
    Edit,
    EditEnum,
    EditStruct,
    Filter,
    NewModule,
    Plus,
    WordWrap,
};

extern std::ostream& operator<<(std::ostream& os, Icon icon);

extern const QIcon& loadIcon(Icon icon);

} // crucible::gui
