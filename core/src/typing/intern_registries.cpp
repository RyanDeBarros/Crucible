#include <crucible/core/typing/intern_registries.hpp>

size_t std::hash<crucible::core::typing::PointerTemplate>::operator()(
    const crucible::core::typing::PointerTemplate& tmpl) const
{
    return tmpl.underlying_type.hash();
}

size_t std::hash<crucible::core::typing::ListTemplate>::operator()(
    const crucible::core::typing::ListTemplate& tmpl) const
{
    return tmpl.element_type.hash();
}

size_t std::hash<crucible::core::typing::SetTemplate>::operator()(
    const crucible::core::typing::SetTemplate& tmpl) const
{
    return tmpl.element_type.hash();
}

size_t std::hash<crucible::core::typing::MapTemplate>::operator()(
    const crucible::core::typing::MapTemplate& tmpl) const
{
    return tmpl.key_type.hash() ^ tmpl.value_type.hash();
}

namespace crucible::core::typing
{

PointerRegistry gPointerRegistry{};

QualifiedType PointerRegistry::underlying_type(Specialization spec) const
{
    return get(spec).underlying_type;
}

ListRegistry gListRegistry{};

QualifiedType ListRegistry::element_type(Specialization spec) const
{
    return get(spec).element_type;
}

SetRegistry gSetRegistry{};

QualifiedType SetRegistry::element_type(Specialization spec) const
{
    return get(spec).element_type;
}

MapRegistry gMapRegistry{};

QualifiedType MapRegistry::key_type(Specialization spec) const
{
    return get(spec).key_type;
}

QualifiedType MapRegistry::value_type(Specialization spec) const
{
    return get(spec).value_type;
}

} // namespace crucible::core::typing
