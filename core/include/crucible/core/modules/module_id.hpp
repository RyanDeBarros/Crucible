#pragma once

#include <utility>

namespace crucible::core::modules
{

class ModuleRegistry;

class ModuleId
{
    friend class ModuleRegistry;

private:
    size_t id_ = 0;

public:
    constexpr ModuleId() = default;

    constexpr static ModuleId null()
    {
        return ModuleId{};
    }

    constexpr bool operator==(const ModuleId& o) const
    {
        return id_ == o.id_;
    }

    constexpr size_t hash() const
    {
        return id_;
    }
};

} // crucible::core::modules

template<>
struct std::hash<crucible::core::modules::ModuleId>
{
    size_t operator()(const crucible::core::modules::ModuleId& m) const
    {
        // TODO move to .cpp
        return m.hash();
    }
};
