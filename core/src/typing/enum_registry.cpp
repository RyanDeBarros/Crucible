
#include <crucible/core/typing/enum_registry.hpp>

#include <stdexcept>
#include <utility>

namespace crucible::core::typing
{

std::vector<EnumRegistry::Slot> EnumRegistry::slots_{ 1 }; // reserve 0 for invalid kind
KindInstance EnumRegistry::free_head_ = 0;
std::mutex EnumRegistry::mutex_;

KindInstance EnumRegistry::new_enum(EnumTemplate tmpl)
{
    std::unique_lock lock(mutex_);

    KindInstance kind;

    if (free_head_ != 0)
    {
        kind = free_head_;
        free_head_ = slots_[kind].next_free;
    }
    else
    {
        kind = static_cast<KindInstance>(slots_.size());
        slots_.emplace_back();
    }

    slots_[kind].definition.emplace(std::move(tmpl));
    slots_[kind].next_free = 0;

    return kind;
}

bool EnumRegistry::delete_enum(KindInstance kind)
{
    std::unique_lock lock(mutex_);

    if (auto* slot = try_get_slot_unsafe(kind))
    {
        slot->definition.reset();
        slot->next_free = free_head_;
        free_head_ = kind;

        return true;
    }
    else
        return false;
}

bool EnumRegistry::enum_exists(KindInstance kind)
{
    std::unique_lock lock(mutex_);

    return kind != 0 && kind < slots_.size() && slots_[kind].definition.has_value();
}

std::string EnumRegistry::enum_title(KindInstance kind)
{
    std::unique_lock lock(mutex_);
    return get_slot_unsafe(kind).definition->title;
}

std::string EnumRegistry::enum_name(KindInstance kind, std::uint64_t index)
{
    std::unique_lock lock(mutex_);

    const auto& names = get_slot_unsafe(kind).definition->names;
    if (index >= names.size())
        throw std::out_of_range("Enum name index out of range");

    return names[index];
}

std::uint64_t EnumRegistry::enum_count(KindInstance kind)
{
    std::unique_lock lock(mutex_);
    return get_slot_unsafe(kind).definition->names.size();
}

EnumRegistry::Slot* EnumRegistry::try_get_slot_unsafe(KindInstance kind)
{
    if (kind == 0 || kind >= slots_.size() || !slots_[kind].definition)
        return nullptr;
    else
        return &slots_[kind];
}

EnumRegistry::Slot& EnumRegistry::get_slot_unsafe(KindInstance kind)
{
    if (auto* slot = try_get_slot_unsafe(kind))
        return *slot;
    else
        throw std::out_of_range("Enum does not exist");
}

} // namespace crucible::core::typing
