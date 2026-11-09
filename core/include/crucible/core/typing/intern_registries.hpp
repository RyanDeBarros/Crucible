#pragma once

#include <crucible/core/typing/intern_registry.hpp>
#include <crucible/core/typing/qualified_type.hpp>

namespace crucible::core::typing
{

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
    : public InternRegistry<PointerTemplate>
{
public:
    QualifiedType underlying_type(Specialization spec) const;
};

extern PointerRegistry gPointerRegistry;

class ListRegistry final
    : public InternRegistry<ListTemplate>
{
public:
    QualifiedType element_type(Specialization spec) const;
};

extern ListRegistry gListRegistry;

class SetRegistry final
    : public InternRegistry<SetTemplate>
{
public:
    QualifiedType element_type(Specialization spec) const;
};

extern SetRegistry gSetRegistry;

class MapRegistry final
    : public InternRegistry<MapTemplate>
{
public:
    QualifiedType key_type(Specialization spec) const;
    QualifiedType value_type(Specialization spec) const;
};

extern MapRegistry gMapRegistry;

} // namespace crucible::core::typing
