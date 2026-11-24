#pragma once

#include <crucible/core/typing/kind.hpp>

#include <memory>
#include <string>

namespace crucible::core::typing
{

struct Simplex
{
    Kind kind;
    Specialization spec; // TODO rename to UserInstance and make uint32_t (remove std:: prefix)

    bool operator==(const Simplex&) const = default;
};

class QualifiedType
{
    constexpr static inline size_t kTemplateCap = 2;

    Simplex simplex_;
    std::unique_ptr<QualifiedType> template_[kTemplateCap]; // TODO instead of heap allocations in every QualifiedType, store a handle of some kind to a central data system of template chains?

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

    std::string repr() const;

    static QualifiedType make_bool();
    static QualifiedType make_int();
    static QualifiedType make_float();
    static QualifiedType make_string();
    static QualifiedType make_pointer(QualifiedType underlying_type);
    static QualifiedType make_enum(Specialization spec);
    static QualifiedType make_list(QualifiedType elem_type);
    static QualifiedType make_set(QualifiedType elem_type);
    static QualifiedType make_map(QualifiedType key_type, QualifiedType value_type);
    static QualifiedType make_struct(Specialization spec);
    static QualifiedType make_joker();
};

} // crucible::core::typing

template<>
struct std::hash<crucible::core::typing::Simplex>
{
    size_t operator()(const crucible::core::typing::Simplex&) const;
};
