#pragma once

#include <ostream>

namespace crucible::gui
{

enum LogColumn : unsigned int
{
    Timestamp,
    Level,
    Category,
    Message,
    _Count
};

extern std::ostream& operator<<(std::ostream& os, LogColumn column);

struct LogColumnMask
{
    uint32_t bits = 0;

    constexpr explicit LogColumnMask() = default;

    constexpr LogColumnMask(LogColumn column)
        : bits(1u << static_cast<std::underlying_type_t<LogColumn>>(column))
    {
    }

    constexpr bool operator==(const LogColumnMask& o) const
    {
        return bits == o.bits;
    }

    constexpr operator bool() const
    {
        return bits != 0;
    }

    constexpr LogColumnMask operator~() const
    {
        LogColumnMask mask{};
        mask.bits = ~bits;
        return mask;
    }

    constexpr LogColumnMask operator&(LogColumnMask o) const
    {
        LogColumnMask mask{};
        mask.bits = bits & o.bits;
        return mask;
    }

    constexpr LogColumnMask operator&(LogColumn column) const
    {
        return *this & LogColumnMask(column);
    }

    constexpr LogColumnMask& operator&=(LogColumnMask o)
    {
        bits &= o.bits;
        return *this;
    }

    constexpr LogColumnMask& operator&=(LogColumn column)
    {
        return *this &= LogColumnMask(column);
    }

    constexpr LogColumnMask operator|(LogColumnMask o) const
    {
        LogColumnMask mask{};
        mask.bits = bits | o.bits;
        return mask;
    }

    constexpr LogColumnMask operator|(LogColumn column) const
    {
        return *this | LogColumnMask(column);
    }

    constexpr LogColumnMask& operator|=(LogColumnMask o)
    {
        bits |= o.bits;
        return *this;
    }

    constexpr LogColumnMask& operator|=(LogColumn column)
    {
        return *this |= LogColumnMask(column);
    }

    constexpr static LogColumnMask full()
    {
        LogColumnMask mask{};
        for (size_t i = 0; i < static_cast<size_t>(LogColumn::_Count); ++i)
            mask.bits |= 1u << i;
        return mask;
    }
};

extern std::ostream& operator<<(std::ostream& os, LogColumnMask mask);

} // crucible::gui
