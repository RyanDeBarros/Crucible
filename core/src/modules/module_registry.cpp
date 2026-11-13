#include <crucible/core/modules/module_registry.hpp>

namespace crucible::core::modules
{

ModuleRegistry gModuleRegistry{};

ModuleId ModuleRegistry::create(typing::Storage storage, Metadata metadata)
{
    std::unique_lock lock(Super::mutex_);
    ModuleId id = Super::add_unsafe({ .storage = std::move(storage), .metadata = std::move(metadata) });
    Super::get_unsafe(id).address = id;
    return id;
}

typing::QualifiedType ModuleRegistry::module_type(ModuleId id) const
{
    return get(id)->storage.type();
}

} // crucible::core::modules
