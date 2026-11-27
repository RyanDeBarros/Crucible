#pragma once

#include <crucible/core/typing/kind.hpp>

#include <crucible/core/utility/registry.hpp>

namespace crucible::core::typing
{

class Type;

struct EnumTemplate
{
    std::string title;
    std::vector<std::string> names;
};

class EnumRegistry final
    : public Registry<EnumTemplate, UserInstance>
{
    using Super = Registry<EnumTemplate, UserInstance>;

public:
    using Lock = Super::Lock;

    std::string enum_title(const Lock& lock, UserInstance user) const;
    std::string enum_name(const Lock& lock, UserInstance user, uint64_t index) const;
    uint64_t enum_count(const Lock& lock, UserInstance user) const;
};

extern EnumRegistry gEnumRegistry;

struct StructTemplate
{
    std::string title;
    std::vector<std::pair<std::string, Type>> fields;

    bool valid() const;
    void assert_valid() const;
};

class StructRegistry final
    : public Registry<StructTemplate, UserInstance>
{
    using Super = Registry<StructTemplate, UserInstance>;

public:
    using Lock = Super::Lock;

    UserInstance add(const Lock& lock, StructTemplate tmpl);

    std::string struct_title(const Lock& lock, UserInstance user) const;
    size_t struct_field_count(const Lock& lock, UserInstance user) const;
    std::string struct_field_name(const Lock& lock, UserInstance user, uint64_t index) const;
    Type struct_field_type(const Lock& lock, UserInstance user, uint64_t index) const;
    Type struct_field_type(const Lock& lock, UserInstance user, const std::string_view name) const;

private:
    using Super::add;
};

extern StructRegistry gStructRegistry;

struct UserRegistryLock
{
    EnumRegistry::Lock enum_lock;
    StructRegistry::Lock struct_lock;

    UserRegistryLock();
};

} // namespace crucible::core::typing
