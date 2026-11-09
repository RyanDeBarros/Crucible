#pragma once

#include <crucible/core/typing/qualified_type.hpp>

#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace crucible::core::typing
{

template<typename Template>
class InternRegistry
{
    struct Slot
    {
        std::optional<Template> tmpl;
        KindInstance next_free = 0;
    };

    std::vector<Slot> slots_{ 1 }; // reserve 0 for invalid kind
    KindInstance free_head_ = 0;
    mutable std::mutex mutex_;

    std::unordered_map<Template, KindInstance> lut_;

public:
    KindInstance intern(Template tmpl)
    {
        std::unique_lock lock(mutex_);

        if (auto it = lut_.find(tmpl); it != lut_.end())
            return it->second;

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

        slots_[kind].tmpl.emplace(tmpl);
        slots_[kind].next_free = 0;
        lut_.emplace(std::move(tmpl), kind);

        return kind;
    }

    bool invalidate(KindInstance kind)
    {
        std::unique_lock lock(mutex_);

        if (auto* slot = try_get_slot_unsafe(kind))
        {
            lut_.erase(*slot->tmpl);

            slot->tmpl.reset();
            slot->next_free = free_head_;
            free_head_ = kind;

            return true;
        }
        else
            return false;
    }

    bool exists(KindInstance kind) const
    {
        std::unique_lock lock(mutex_);
        return try_get_slot_unsafe(kind) != nullptr;
    }

    const Template& get(KindInstance kind) const
    {
        std::unique_lock lock(mutex_);
        return *get_slot_unsafe(kind).tmpl;
    }

private:
    const Slot* try_get_slot_unsafe(KindInstance kind) const
    {
        if (kind == 0 || kind >= slots_.size() || !slots_[kind].tmpl)
            return nullptr;
        else
            return &slots_[kind];
    }

    Slot* try_get_slot_unsafe(KindInstance kind)
    {
        if (kind == 0 || kind >= slots_.size() || !slots_[kind].tmpl)
            return nullptr;
        else
            return &slots_[kind];
    }

    const Slot& get_slot_unsafe(KindInstance kind) const
    {
        if (auto* slot = try_get_slot_unsafe(kind))
            return *slot;
        else
            throw std::logic_error("Specialization does not exist");
    }
};

} // crucible::core::typing
