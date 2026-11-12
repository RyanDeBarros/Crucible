#include <crucible/core/modules/module_registry.hpp>

namespace crucible::core::modules
{

ModuleRegistry gModuleRegistry{};


ModuleId ModuleRegistry::add(Module module)
{
    std::unique_lock lock(mutex_);

    ModuleId id;

    if (free_head_ != ModuleId::null())
    {
        id = free_head_;
        free_head_ = slots_[id.id_].next_free;
    }
    else
    {
        id.id_ = slots_.size();
        slots_.emplace_back();
    }

    slots_[id.id_].module.emplace(std::move(module));
    slots_[id.id_].next_free = ModuleId::null();

    return id;
}

bool ModuleRegistry::remove(ModuleId id)
{
    std::unique_lock lock(mutex_);

    if (auto* slot = try_get_slot_unsafe(id))
    {
        slot->module.reset();
        slot->next_free = free_head_;
        free_head_ = id;

        return true;
    }
    else
        return false;
}

bool ModuleRegistry::exists(ModuleId id) const
{
    std::unique_lock lock(mutex_);
    return try_get_slot_unsafe(id) != nullptr;
}

const Module& ModuleRegistry::get(ModuleId id) const
{
    std::unique_lock lock(mutex_);
    return *get_slot_unsafe(id).module;
}

const typing::QualifiedType& ModuleRegistry::type(ModuleId id) const
{
    return get(id).storage.type();
}

const ModuleRegistry::Slot* ModuleRegistry::try_get_slot_unsafe(ModuleId id) const
{
    if (id.id_ == 0 || id.id_ >= slots_.size() || !slots_[id.id_].module)
        return nullptr;
    else
        return &slots_[id.id_];
}

ModuleRegistry::Slot* ModuleRegistry::try_get_slot_unsafe(ModuleId id)
{
    if (id.id_ == 0 || id.id_ >= slots_.size() || !slots_[id.id_].module)
        return nullptr;
    else
        return &slots_[id.id_];
}

const ModuleRegistry::Slot& ModuleRegistry::get_slot_unsafe(ModuleId id) const
{
    if (auto* slot = try_get_slot_unsafe(id))
        return *slot;
    else
        throw std::logic_error("ModuleId does not exist");
}

} // crucible::core::modules
