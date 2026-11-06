#pragma once

#include <ostream>

namespace crucible::core
{

enum class LogLevel : unsigned int
{
    Debug,
    Info,
    Warning,
    Error,
    _Count
};

extern std::ostream& operator<<(std::ostream& os, LogLevel level);

struct LogLevelMask
{
    uint32_t bits = 0;

    constexpr explicit LogLevelMask() = default;

    constexpr LogLevelMask(LogLevel level)
        : bits(1u << static_cast<std::underlying_type_t<LogLevel>>(level))
    {
    }

    constexpr bool operator==(const LogLevelMask& o) const
    {
        return bits == o.bits;
    }

    constexpr operator bool() const
    {
        return bits != 0;
    }

    constexpr LogLevelMask operator~() const
    {
        LogLevelMask mask{};
        mask.bits = ~bits;
        return mask;
    }

    constexpr LogLevelMask operator&(LogLevelMask o) const
    {
        LogLevelMask mask{};
        mask.bits = bits & o.bits;
        return mask;
    }

    constexpr LogLevelMask operator&(LogLevel level) const
    {
        return *this & LogLevelMask(level);
    }

    constexpr LogLevelMask& operator&=(LogLevelMask o)
    {
        bits &= o.bits;
        return *this;
    }

    constexpr LogLevelMask& operator&=(LogLevel level)
    {
        return *this &= LogLevelMask(level);
    }

    constexpr LogLevelMask operator|(LogLevelMask o) const
    {
        LogLevelMask mask{};
        mask.bits = bits | o.bits;
        return mask;
    }

    constexpr LogLevelMask operator|(LogLevel level) const
    {
        return *this | LogLevelMask(level);
    }

    constexpr LogLevelMask& operator|=(LogLevelMask o)
    {
        bits |= o.bits;
        return *this;
    }

    constexpr LogLevelMask& operator|=(LogLevel level)
    {
        return *this |= LogLevelMask(level);
    }

    constexpr static LogLevelMask full()
    {
        LogLevelMask mask{};
        for (size_t i = 0; i < static_cast<size_t>(LogLevel::_Count); ++i)
            mask.bits |= 1u << i;
        return mask;
    }
};

extern std::ostream& operator<<(std::ostream& os, LogLevelMask mask);

} // crucible::core
