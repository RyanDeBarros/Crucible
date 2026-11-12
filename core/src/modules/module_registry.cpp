#include <crucible/core/modules/module_registry.hpp>

namespace crucible::core::modules
{

ModuleRegistry gModuleRegistry{};

typing::QualifiedType ModuleRegistry::module_type(ModuleId id) const
{
    return get(id)->storage.type();
}

} // crucible::core::modules
