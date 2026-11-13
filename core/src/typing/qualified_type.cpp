#include <crucible/core/typing/qualified_type.hpp>

#include <crucible/core/typing/intern_registries.hpp>
#include <crucible/core/typing/nominal_registries.hpp>

namespace crucible::core::typing
{

QualifiedType::QualifiedType(Kind base_type, Specialization spec)
    : kind_(base_type)
    , spec_(spec)
{
}

Kind QualifiedType::kind() const
{
    return kind_;
}

Specialization QualifiedType::specialization() const
{
    return spec_;
}

size_t QualifiedType::hash() const
{
    return std::hash<Kind>{}(kind_) ^ std::hash<Specialization>{}(spec_);
}

QualifiedType QualifiedType::make_bool()
{
    return QualifiedType(Kind::Bool, 0);
}

QualifiedType QualifiedType::make_int()
{
    return QualifiedType(Kind::Int, 0);
}

QualifiedType QualifiedType::make_float()
{
    return QualifiedType(Kind::Float, 0);
}

QualifiedType QualifiedType::make_string()
{
    return QualifiedType(Kind::String, 0);
}

QualifiedType QualifiedType::make_pointer(QualifiedType underlying_type)
{
    return QualifiedType(
        Kind::Pointer,
        gPointerRegistry.intern({ .underlying_type = underlying_type }));
}

QualifiedType QualifiedType::make_enum(Specialization spec)
{
    return QualifiedType(Kind::Enum, spec);
}

QualifiedType QualifiedType::make_list(QualifiedType elem_type)
{
    return QualifiedType(
        Kind::List,
        gListRegistry.intern({ .element_type = std::move(elem_type) }));
}

QualifiedType QualifiedType::make_set(QualifiedType elem_type)
{
    return QualifiedType(
        Kind::Set,
        gSetRegistry.intern({ .element_type = std::move(elem_type) }));
}

QualifiedType QualifiedType::make_map(QualifiedType key_type, QualifiedType value_type)
{
    return QualifiedType(
        Kind::Map,
        gMapRegistry.intern({ .key_type = std::move(key_type), .value_type = std::move(value_type) }));
}

QualifiedType QualifiedType::make_struct(Specialization spec)
{
    return QualifiedType(Kind::Struct, spec);
}

QualifiedType QualifiedType::make_joker()
{
    return QualifiedType(Kind::Joker, 0);
}

} // crucible::core::typing
