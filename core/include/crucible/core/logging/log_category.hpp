#pragma once

#include <string>

namespace crucible::core
{

struct LogCategory
{
    std::string text;

    bool operator==(const LogCategory&) const = default;
};

extern const LogCategory kCore;
extern const LogCategory kIO;

} // crucible::core
