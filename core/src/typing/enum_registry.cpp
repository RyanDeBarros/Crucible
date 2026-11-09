#include <crucible/core/typing/enum_registry.hpp>

namespace crucible::core::typing
{

std::string EnumRegistry::enum_title(KindInstance kind) const
{
    return get(kind).title;
}

std::string EnumRegistry::enum_name(KindInstance kind, std::uint64_t index) const
{
    const auto& names = get(kind).names;
    if (index >= names.size())
        throw std::out_of_range("Enum name index out of range");

    return names[index];
}

std::uint64_t EnumRegistry::enum_count(KindInstance kind) const
{
    return get(kind).names.size();
}

} // namespace crucible::core::typing
