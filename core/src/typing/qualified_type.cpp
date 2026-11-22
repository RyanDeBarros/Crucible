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

std::string QualifiedType::repr() const
{
    switch (kind_)
    {
    case crucible::core::typing::Kind::Bool:
        return "bool";
    case crucible::core::typing::Kind::Int:
        return "int";
    case crucible::core::typing::Kind::Float:
        return "float";
    case crucible::core::typing::Kind::String:
        return "string";
    case crucible::core::typing::Kind::Pointer:
        return gPointerRegistry.underlying_type(gPointerRegistry.lock(), spec_).repr() + "*";
    case crucible::core::typing::Kind::Enum:
        return "#" + gEnumRegistry.enum_title(gEnumRegistry.lock(), spec_);
    case crucible::core::typing::Kind::List:
        return "list[" + gListRegistry.element_type(gListRegistry.lock(), spec_).repr() + "]";
    case crucible::core::typing::Kind::Set:
        return "set[" + gSetRegistry.element_type(gSetRegistry.lock(), spec_).repr() + "]";
    case crucible::core::typing::Kind::Map:
    {
        auto lock = gMapRegistry.lock();
        return "map[" + gMapRegistry.key_type(lock, spec_).repr()
            + ", " + gMapRegistry.value_type(lock, spec_).repr() + "]";
    }
    case crucible::core::typing::Kind::Struct:
        return "!" + gStructRegistry.struct_title(gStructRegistry.lock(), spec_);
    case crucible::core::typing::Kind::Joker:
        return "joker";
    default:
        throw std::runtime_error("unsupported type");
    }
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
        gPointerRegistry.intern(gPointerRegistry.lock(), {.underlying_type = underlying_type}));
}

QualifiedType QualifiedType::make_enum(Specialization spec)
{
    return QualifiedType(Kind::Enum, spec);
}

QualifiedType QualifiedType::make_list(QualifiedType elem_type)
{
    return QualifiedType(
        Kind::List,
        gListRegistry.intern(gListRegistry.lock(), {.element_type = std::move(elem_type)}));
}

QualifiedType QualifiedType::make_set(QualifiedType elem_type)
{
    return QualifiedType(
        Kind::Set,
        gSetRegistry.intern(gSetRegistry.lock(), {.element_type = std::move(elem_type)}));
}

QualifiedType QualifiedType::make_map(QualifiedType key_type, QualifiedType value_type)
{
    return QualifiedType(
        Kind::Map,
        gMapRegistry.intern(
            gMapRegistry.lock(),
            {.key_type = std::move(key_type), .value_type = std::move(value_type)}));
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
