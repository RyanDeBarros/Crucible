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
    UserInstance user;

    bool operator==(const Simplex&) const = default;
};

class QualifiedType
{
    Simplex simplex_;
    std::array<std::unique_ptr<QualifiedType>, 2> template_; // TODO instead of heap allocations in every QualifiedType, store a handle of some kind to a central data system of template chains? Would also make comparisons much quicker and support caching.

    QualifiedType(Simplex simplex);

public:
    QualifiedType(const QualifiedType&);
    QualifiedType(QualifiedType&&) noexcept;
    QualifiedType& operator=(const QualifiedType&);
    QualifiedType& operator=(QualifiedType&&) noexcept;

    bool operator==(const QualifiedType&) const;

    std::unique_ptr<QualifiedType> clone() const;

    Simplex simplex() const;
    const QualifiedType* underlying(size_t idx) const;
    size_t hash() const;

    static QualifiedType make_bool();
    static QualifiedType make_int();
    static QualifiedType make_float();
    static QualifiedType make_string();
    static QualifiedType make_pointer(QualifiedType underlying_type);
    static QualifiedType make_enum(UserInstance user);
    static QualifiedType make_list(QualifiedType elem_type);
    static QualifiedType make_set(QualifiedType elem_type);
    static QualifiedType make_map(QualifiedType key_type, QualifiedType value_type);
    static QualifiedType make_struct(UserInstance user);
    static QualifiedType make_joker();
};

} // crucible::core::typing

template<>
struct std::hash<crucible::core::typing::Simplex>
{
    size_t operator()(const crucible::core::typing::Simplex&) const;
};
