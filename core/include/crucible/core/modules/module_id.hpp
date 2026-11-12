#pragma once

#include <utility>

namespace crucible::core::modules
{

struct ModuleId
{
    size_t id = 0;

    constexpr static ModuleId null()
    {
        return ModuleId{};
    }

    constexpr bool operator==(const ModuleId& o) const
    {
        return id == o.id;
    }
};

} // crucible::core::modules

template<>
struct std::hash<crucible::core::modules::ModuleId>
{
    size_t operator()(const crucible::core::modules::ModuleId& m) const
    {
        return std::hash<size_t>{}(m.id);
    }
};
