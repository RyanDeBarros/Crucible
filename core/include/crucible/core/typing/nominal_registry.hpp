#pragma once

#include <crucible/core/typing/qualified_type.hpp>

#include <mutex>
#include <optional>
#include <stdexcept>
#include <vector>

namespace crucible::core::typing
{

template<typename Template>
class NominalRegistry
{
    struct Slot
    {
        std::optional<Template> definition;
        KindInstance next_free = 0;
    };

    std::vector<Slot> slots_{ 1 }; // reserve 0 for invalid kind
    KindInstance free_head_ = 0;
    mutable std::mutex mutex_;

public:
    KindInstance add(Template tmpl)
    {
        assert_valid(tmpl);

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

    bool remove(KindInstance kind)
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

    bool exists(KindInstance kind) const
    {
        std::unique_lock lock(mutex_);
        return try_get_slot_unsafe(kind) != nullptr;
    }

    const Template& get(KindInstance kind) const
    {
        std::unique_lock lock(mutex_);
        return *get_slot_unsafe(kind).definition;
    }

protected:
    virtual void assert_valid(const Template& tmpl) const
    {
    }

private:
    const Slot* try_get_slot_unsafe(KindInstance kind) const
    {
        if (kind == 0 || kind >= slots_.size() || !slots_[kind].definition)
            return nullptr;
        else
            return &slots_[kind];
    }

    Slot* try_get_slot_unsafe(KindInstance kind)
    {
        if (kind == 0 || kind >= slots_.size() || !slots_[kind].definition)
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
