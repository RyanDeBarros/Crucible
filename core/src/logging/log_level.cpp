#include <crucible/core/logging/log_level.hpp>

namespace crucible::core
{

std::ostream& operator<<(std::ostream& os, LogLevel level)
{
    switch (level)
    {
    case LogLevel::Debug:
        return os << "DEBUG";

    case LogLevel::Info:
        return os << "INFO";

    case LogLevel::Warning:
        return os << "WARNING";

    case LogLevel::Error:
        return os << "ERROR";

    default:
        return os;
    }
}

std::ostream& operator<<(std::ostream& os, LogLevelMask mask)
{
    if (mask == LogLevelMask::full())
        return os << "[FULL]";

    bool prev_elem = false;

    os << "[";
    for (size_t i = 0; i < static_cast<size_t>(LogLevel::_Count); ++i)
    {
        if (mask & static_cast<LogLevel>(i))
        {
            if (prev_elem)
                os << ", ";

            prev_elem = true;
            os << static_cast<LogLevel>(i);
        }
    }

    return os << "]";
}

} // crucible::core
