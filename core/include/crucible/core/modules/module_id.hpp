#pragma once

namespace crucible::core
{

struct ModuleId
{
    size_t id = 0;

    constexpr static ModuleId null()
    {
        return ModuleId{};
    }
};

} // crucible::core
