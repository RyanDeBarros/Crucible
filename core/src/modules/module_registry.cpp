#include <crucible/core/modules/module_registry.hpp>

// TODO unify three registry designs (nominal, intern, module) and just extend it for specific features.

namespace crucible::core::modules
{

ModuleRegistry gModuleRegistry{};

const typing::QualifiedType& ModuleRegistry::module_type(ModuleId id) const
{
    return get(id).storage.type();
}

} // crucible::core::modules
