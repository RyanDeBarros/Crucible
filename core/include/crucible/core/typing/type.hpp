#pragma once

#include <crucible/core/typing/kind.hpp>

#include <array>
#include <memory>
#include <string>

namespace crucible::core::typing
{

struct Simplex
{
    Kind kind;
    UserInstance user = 0;

    bool operator==(const Simplex&) const = default;

    size_t argcount() const;
};

namespace detail
{

using TypeNodeId = size_t;

} // detail

class Type
{
    detail::TypeNodeId id_ = 0;

    Type(detail::TypeNodeId id);

public:
    Type(const Type&);
    Type(Type&&) noexcept;
    ~Type();

    Type& operator=(const Type&);
    Type& operator=(Type&&) noexcept;

    bool operator==(const Type&) const;
    bool valid() const;

    Simplex simplex() const;
    Type underlying(size_t idx) const;

    size_t hash() const;

    static Type make_bool();
    static Type make_int();
    static Type make_float();
    static Type make_string();
    static Type make_pointer(const Type& underlying_type);
    static Type make_enum(UserInstance user);
    static Type make_list(const Type& elem_type);
    static Type make_set(const Type& elem_type);
    static Type make_map(const Type& key_type, const Type& value_type);
    static Type make_struct(UserInstance user);
    static Type make_joker();
};

} // crucible::core::typing
