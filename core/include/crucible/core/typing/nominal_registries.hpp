#pragma once

#include <crucible/core/typing/nominal_registry.hpp>

namespace crucible::core::typing
{

class QualifiedType;

struct EnumTemplate
{
    std::string title;
    std::vector<std::string> names;
};

class EnumRegistry final
    : public NominalRegistry<EnumTemplate>
{
public:
    std::string enum_title(Specialization spec) const;
    std::string enum_name(Specialization spec, std::uint64_t index) const;
    std::uint64_t enum_count(Specialization spec) const;
};

extern EnumRegistry gEnumRegistry;

struct StructTemplate
{
    std::string title;
    std::vector<std::pair<std::string, QualifiedType>> fields;

    bool valid() const;
    void assert_valid() const;
};

class StructRegistry final
    : public NominalRegistry<StructTemplate>
{
public:
    std::string struct_title(Specialization spec) const;
    size_t struct_field_count(Specialization spec) const;
    std::string struct_field_name(Specialization spec, std::uint64_t index) const;
    QualifiedType struct_field_type(Specialization spec, std::uint64_t index) const;
    QualifiedType struct_field_type(Specialization spec, const std::string_view name) const;

protected:
    void assert_valid(const StructTemplate& tmpl) const override;
};

extern StructRegistry gStructRegistry;

} // namespace crucible::core::typing
