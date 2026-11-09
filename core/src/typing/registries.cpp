#include <crucible/core/typing/registries.hpp>

#include <stdexcept>
#include <unordered_set>

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

std::string StructRegistry::struct_title(KindInstance kind) const
{
    return get(kind).title;
}

size_t StructRegistry::struct_field_count(KindInstance kind) const
{
    return get(kind).fields.size();
}

std::string StructRegistry::struct_field_name(KindInstance kind, std::uint64_t index) const
{
    const auto& fields = get(kind).fields;
    if (index >= fields.size())
        throw std::out_of_range("Struct field index out of range");

    return fields[index].first;
}

QualifiedType StructRegistry::struct_field_type(KindInstance kind, std::uint64_t index) const
{
    const auto& fields = get(kind).fields;
    if (index >= fields.size())
        throw std::out_of_range("Struct field index out of range");

    return fields[index].second;
}

QualifiedType StructRegistry::struct_field_type(KindInstance kind, const std::string_view name) const
{
    const auto& fields = get(kind).fields;
    for (const auto& [fname, ftype] : fields)
    {
        if (fname == name)
            return ftype;
    }

    throw std::logic_error("Struct field does not exist");
}

void StructRegistry::assert_valid(const StructTemplate& tmpl) const
{
    tmpl.assert_valid();
}

} // namespace crucible::core::typing
