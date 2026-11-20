#include <crucible/core/modules/module_registry.hpp>

namespace crucible::core::modules
{

ModuleRegistry gModuleRegistry{};

ModuleId ModuleRegistry::create(const Lock& lock, typing::Storage storage, Metadata metadata)
{
    lock.assert_valid(this);
    ModuleId id = Super::add_unsafe({ .storage = std::move(storage), .metadata = std::move(metadata) });
    Super::get_unsafe(id).address = id;
    return id;
}

typing::QualifiedType ModuleRegistry::module_type(const Lock& lock, ModuleId id) const
{
    return get(lock, id).storage.type();
}

} // crucible::core::modules
