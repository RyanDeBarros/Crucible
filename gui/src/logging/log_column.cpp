#include <crucible/gui/logging/log_column.hpp>

namespace crucible::gui
{

std::ostream& operator<<(std::ostream& os, LogColumn column)
{
    switch (column)
    {
    case LogColumn::Timestamp:
        return os << "TIMESTAMP";

    case LogColumn::Level:
        return os << "LEVEL";

    case LogColumn::Category:
        return os << "CATEGORY";

    case LogColumn::Message:
        return os << "MESSAGE";

    default:
        return os;
    }
}

std::ostream& operator<<(std::ostream& os, LogColumnMask mask)
{
    if (mask == LogColumnMask::full())
        return os << "[FULL]";

    bool prev_elem = false;

    os << "[";
    for (size_t i = 0; i < static_cast<size_t>(LogColumn::_Count); ++i)
    {
        if (static_cast<LogColumn>(i) == LogColumn::Message)
            continue;

        if (mask & static_cast<LogColumn>(i))
        {
            if (prev_elem)
                os << ", ";

            prev_elem = true;
            os << static_cast<LogColumn>(i);
        }
    }

    return os << "]";
}

} // crucible::gui
