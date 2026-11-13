#include <crucible/core/typing/nominal_registries.hpp>

#include <crucible/core/typing/qualified_type.hpp>

#include <unordered_set>

namespace crucible::core::typing
{

EnumRegistry gEnumRegistry{};

std::string EnumRegistry::enum_title(Specialization spec) const
{
    return get(spec)->title;
}

std::string EnumRegistry::enum_name(Specialization spec, std::uint64_t index) const
{
    const auto& names = get(spec)->names;
    if (index >= names.size())
        throw std::out_of_range("Enum name index out of range");

    return names[index];
}

std::uint64_t EnumRegistry::enum_count(Specialization spec) const
{
    return get(spec)->names.size();
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

Specialization StructRegistry::add(StructTemplate tmpl)
{
    tmpl.assert_valid();
    return Super::add(std::move(tmpl));
}

std::string StructRegistry::struct_title(Specialization spec) const
{
    return get(spec)->title;
}

size_t StructRegistry::struct_field_count(Specialization spec) const
{
    return get(spec)->fields.size();
}

std::string StructRegistry::struct_field_name(Specialization spec, std::uint64_t index) const
{
    const auto& fields = get(spec)->fields;
    if (index >= fields.size())
        throw std::out_of_range("Struct field index out of range");

    return fields[index].first;
}

QualifiedType StructRegistry::struct_field_type(Specialization spec, std::uint64_t index) const
{
    const auto& fields = get(spec)->fields;
    if (index >= fields.size())
        throw std::out_of_range("Struct field index out of range");

    return fields[index].second;
}

QualifiedType StructRegistry::struct_field_type(Specialization spec, const std::string_view name) const
{
    const auto& fields = get(spec)->fields;
    for (const auto& [fname, ftype] : fields)
    {
        if (fname == name)
            return ftype;
    }

    throw std::logic_error("Struct field does not exist");
}

} // namespace crucible::core::typing
