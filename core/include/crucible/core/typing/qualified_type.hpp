#pragma once

#include <crucible/core/typing/kind.hpp>

namespace crucible::core::typing
{

class QualifiedType
{
    Kind kind_;
    Specialization spec_; // pointer, enum, list, set, map, struct

private:
    QualifiedType(Kind base_type, Specialization spec);

public:
    bool operator==(const QualifiedType&) const = default;
    bool operator!=(const QualifiedType&) const = default;

    Kind kind() const;
    Specialization specialization() const;
    size_t hash() const;

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
