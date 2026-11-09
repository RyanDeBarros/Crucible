#pragma once

#include <memory>

namespace crucible::core::typing
{

enum class Kind
{
    Bool,
    Int,
    Float,
    String,
    Pointer,
    Enum,

    List,
    Set,
    Map,
    Struct,

    Joker
};

// TODO rename to something like Specialization
using KindInstance = size_t;

class QualifiedType
{
    Kind base_type_;
    std::unique_ptr<QualifiedType> key_type_; // pointer, list, set, map
    std::unique_ptr<QualifiedType> value_type_; // map
    KindInstance instance_; // enum, struct

private:
    QualifiedType(
        Kind base_type,
        std::unique_ptr<QualifiedType> key_type,
        std::unique_ptr<QualifiedType> value_type,
        KindInstance instance);

public:
    QualifiedType(const QualifiedType&);
    QualifiedType(QualifiedType&&) noexcept;
    
    QualifiedType& operator=(const QualifiedType&);
    QualifiedType& operator=(QualifiedType&&) noexcept;

    bool operator==(const QualifiedType&) const;
    bool operator!=(const QualifiedType&) const;

    Kind base_type() const;
    const QualifiedType* key_type() const;
    const QualifiedType* value_type() const;
    KindInstance instance() const;

    std::unique_ptr<QualifiedType> clone() const;
    std::unique_ptr<QualifiedType> to_ptr() &&;
    
    static QualifiedType make_bool();
    static QualifiedType make_int();
    static QualifiedType make_float();
    static QualifiedType make_string();
    static QualifiedType make_pointer(QualifiedType pointer_type);
    static QualifiedType make_enum(KindInstance instance);
    static QualifiedType make_list(QualifiedType elem_type);
    static QualifiedType make_set(QualifiedType elem_type);
    static QualifiedType make_map(QualifiedType key_type, QualifiedType map_type);
    static QualifiedType make_struct(KindInstance instance);
    static QualifiedType make_joker();
};

} // crucible::core::typing
