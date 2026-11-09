#pragma once

#include <crucible/core/typing/nominal_registry.hpp>

namespace crucible::core::typing
{

struct EnumTemplate
{
    std::string title;
    std::vector<std::string> names;
};

class EnumRegistry final
    : public NominalRegistry<EnumTemplate>
{
public:
    std::string enum_title(KindInstance kind) const;
    std::string enum_name(KindInstance kind, std::uint64_t index) const;
    std::uint64_t enum_count(KindInstance kind) const;
};

inline EnumRegistry gEnumRegistry{};

} // namespace crucible::core::typing
