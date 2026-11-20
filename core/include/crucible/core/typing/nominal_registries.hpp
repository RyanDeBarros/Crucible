#pragma once

#include <crucible/core/typing/kind.hpp>

#include <crucible/core/utility/registry.hpp>

namespace crucible::core::typing
{

class QualifiedType;

template<typename Template>
class NominalRegistry
    : private Registry<Template, Specialization>
{
    using Super = Registry<Template, Specialization>;

public:
    using Lock = Super::Lock;

    using Super::lock;
    using Super::add;
    using Super::remove;
    using Super::exists;
    using Super::get;
    using Super::range;
    using Super::peek_next_handle;
};

struct EnumTemplate
{
    std::string title;
    std::vector<std::string> names;
};

class EnumRegistry final
    : private NominalRegistry<EnumTemplate>
{
    using Super = NominalRegistry<EnumTemplate>;

public:
    using Lock = Super::Lock;

    using Super::lock;
    using Super::add;
    using Super::remove;
    using Super::exists;
    using Super::get;
    using Super::range;
    using Super::peek_next_handle;

    std::string enum_title(const Lock& lock, Specialization spec) const;
    std::string enum_name(const Lock& lock, Specialization spec, std::uint64_t index) const;
    std::uint64_t enum_count(const Lock& lock, Specialization spec) const;
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
    : private NominalRegistry<StructTemplate>
{
    using Super = NominalRegistry<StructTemplate>;

public:
    using Lock = Super::Lock;

    Specialization add(const Lock& lock, StructTemplate tmpl);
    using Super::lock;
    using Super::remove;
    using Super::exists;
    using Super::get;
    using Super::range;
    using Super::peek_next_handle;

    std::string struct_title(const Lock& lock, Specialization spec) const;
    size_t struct_field_count(const Lock& lock, Specialization spec) const;
    std::string struct_field_name(const Lock& lock, Specialization spec, std::uint64_t index) const;
    QualifiedType struct_field_type(const Lock& lock, Specialization spec, std::uint64_t index) const;
    QualifiedType struct_field_type(const Lock& lock, Specialization spec, const std::string_view name) const;
};

extern StructRegistry gStructRegistry;

} // namespace crucible::core::typing
