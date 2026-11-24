#include <crucible/core/typing/nominal_registries.hpp>

#include <crucible/core/typing/qualified_type.hpp>

#include <unordered_set>

namespace crucible::core::typing
{

EnumRegistry gEnumRegistry{};

std::string EnumRegistry::enum_title(const Lock& lock, UserInstance user) const
{
    return get(lock, user).title;
}

std::string EnumRegistry::enum_name(const Lock& lock, UserInstance user, uint64_t index) const
{
    const auto& names = get(lock, user).names;
    if (index >= names.size())
        throw std::out_of_range("Enum name index out of range");

    return names[index];
}

uint64_t EnumRegistry::enum_count(const Lock& lock, UserInstance user) const
{
    return get(lock, user).names.size();
}

StructRegistry gStructRegistry{};

bool StructTemplate::valid() const
{
    std::unordered_set<std::string> names;
    for (const auto& [name, type] : fields)
    {
        if (!names.insert(name).second)
            return false;
    }

    return true;
}

void StructTemplate::assert_valid() const
{
    if (!valid())
        throw std::logic_error("Struct template has duplicate field names");
}

UserInstance StructRegistry::add(const Lock& lock, StructTemplate tmpl)
{
    tmpl.assert_valid();
    return Super::add(lock, std::move(tmpl));
}

std::string StructRegistry::struct_title(const Lock& lock, UserInstance user) const
{
    return get(lock, user).title;
}

size_t StructRegistry::struct_field_count(const Lock& lock, UserInstance user) const
{
    return get(lock, user).fields.size();
}

std::string StructRegistry::struct_field_name(const Lock& lock, UserInstance user, uint64_t index) const
{
    const auto& fields = get(lock, user).fields;
    if (index >= fields.size())
        throw std::out_of_range("Struct field index out of range");

    return fields[index].first;
}

QualifiedType StructRegistry::struct_field_type(const Lock& lock, UserInstance user, uint64_t index) const
{
    const auto& fields = get(lock, user).fields;
    if (index >= fields.size())
        throw std::out_of_range("Struct field index out of range");

    return fields[index].second;
}

QualifiedType StructRegistry::struct_field_type(const Lock& lock, UserInstance user, const std::string_view name) const
{
    const auto& fields = get(lock, user).fields;
    for (const auto& [fname, ftype] : fields)
    {
        if (fname == name)
            return ftype;
    }

    throw std::logic_error("Struct field does not exist");
}

} // namespace crucible::core::typing
