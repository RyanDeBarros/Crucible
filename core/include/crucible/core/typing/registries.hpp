#pragma once

#include <crucible/core/typing/nominal_registry.hpp>

namespace crucible::core::typing
{

// /++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\

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

// \++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/


// /++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\

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
    std::string struct_title(KindInstance kind) const;
    size_t struct_field_count(KindInstance kind) const;
    std::string struct_field_name(KindInstance kind, std::uint64_t index) const;
    QualifiedType struct_field_type(KindInstance kind, std::uint64_t index) const;
    QualifiedType struct_field_type(KindInstance kind, const std::string_view name) const;

protected:
    void assert_valid(const StructTemplate& tmpl) const override;
};

inline StructRegistry gStructRegistry{};

// \++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/


} // namespace crucible::core::typing
