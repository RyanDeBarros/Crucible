#pragma once

#include <crucible/core/typing/kind.hpp>
#include <crucible/core/typing/qualified_type.hpp>

#include <crucible/core/utility/registry.hpp>

#include <unordered_map>

namespace crucible::core::typing
{

template<typename Template>
class InternRegistry
    : private Registry<Template, Specialization>
{
    using Super = Registry<Template, Specialization>;

private:
    std::unordered_map<Template, Specialization> lut_;

public:
    Specialization intern(Template tmpl)
    {
        std::unique_lock lock(Super::mutex_);

        if (auto it = lut_.find(tmpl); it != lut_.end())
            return it->second;

        if (auto spec = Super::add_unsafe(tmpl))
        {
            lut_.emplace(std::move(tmpl), spec);
            return spec;
        }
        else
            return {};
    }

    bool invalidate(Specialization spec)
    {
        std::unique_lock lock(Super::mutex_);
        if (Super::exists_unsafe(spec))
        {
            lut_.erase(Super::get_unsafe(spec));
            return Super::erase_unsafe(spec);
        }
        else
            return false;
    }

    using Super::exists;
    using Super::get;
};

struct PointerTemplate
{
    QualifiedType underlying_type;

    bool operator==(const PointerTemplate&) const = default;
};

struct ListTemplate
{
    QualifiedType element_type;

    bool operator==(const ListTemplate&) const = default;
};

struct SetTemplate
{
    QualifiedType element_type;

    bool operator==(const SetTemplate&) const = default;
};

struct MapTemplate
{
    QualifiedType key_type;
    QualifiedType value_type;

    bool operator==(const MapTemplate&) const = default;
};

} // crucible::core::typing

template<>
struct std::hash<crucible::core::typing::PointerTemplate>
{
    size_t operator()(const crucible::core::typing::PointerTemplate& tmpl) const;
};

template<>
struct std::hash<crucible::core::typing::ListTemplate>
{
    size_t operator()(const crucible::core::typing::ListTemplate& tmpl) const;
};

template<>
struct std::hash<crucible::core::typing::SetTemplate>
{
    size_t operator()(const crucible::core::typing::SetTemplate& tmpl) const;
};

template<>
struct std::hash<crucible::core::typing::MapTemplate>
{
    size_t operator()(const crucible::core::typing::MapTemplate& tmpl) const;
};

namespace crucible::core::typing
{

class PointerRegistry final
    : private InternRegistry<PointerTemplate>
{
    using Super = InternRegistry<PointerTemplate>;

public:
    using Super::intern;
    using Super::invalidate;
    using Super::exists;

    QualifiedType underlying_type(Specialization spec) const;
};

extern PointerRegistry gPointerRegistry;

class ListRegistry final
    : private InternRegistry<ListTemplate>
{
    using Super = InternRegistry<ListTemplate>;

public:
    using Super::intern;
    using Super::invalidate;
    using Super::exists;

    QualifiedType element_type(Specialization spec) const;
};

extern ListRegistry gListRegistry;

class SetRegistry final
    : private InternRegistry<SetTemplate>
{
    using Super = InternRegistry<SetTemplate>;

public:
    using Super::intern;
    using Super::invalidate;
    using Super::exists;

    QualifiedType element_type(Specialization spec) const;
};

extern SetRegistry gSetRegistry;

class MapRegistry final
    : private InternRegistry<MapTemplate>
{
    using Super = InternRegistry<MapTemplate>;

public:
    using Super::intern;
    using Super::invalidate;
    using Super::exists;

    QualifiedType key_type(Specialization spec) const;
    QualifiedType value_type(Specialization spec) const;
};

extern MapRegistry gMapRegistry;

} // namespace crucible::core::typing
