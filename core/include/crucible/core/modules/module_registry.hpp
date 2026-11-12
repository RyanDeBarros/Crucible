#pragma once

#include <crucible/core/modules/module.hpp>

#include <mutex>
#include <optional>

namespace crucible::core::modules
{

class ModuleRegistry
{
    struct Slot
    {
        std::optional<Module> module; // TODO here and in other registries, use unique_ptr or double-indexing with multiple vectors for stable references, since get() return const&.
        ModuleId next_free = ModuleId::null();
    };

    std::vector<Slot> slots_{ 1 }; // reserve 0 for invalid id
    ModuleId free_head_ = ModuleId::null();
    mutable std::mutex mutex_;

public:
    ModuleId add(Module module);
    bool remove(ModuleId id);
    bool exists(ModuleId id) const;
    const Module& get(ModuleId id) const;

    const typing::QualifiedType& type(ModuleId id) const;

private:
    const Slot* try_get_slot_unsafe(ModuleId id) const;
    Slot* try_get_slot_unsafe(ModuleId id);
    const Slot& get_slot_unsafe(ModuleId id) const;
};

extern ModuleRegistry gModuleRegistry;

} // crucible::core::modules
