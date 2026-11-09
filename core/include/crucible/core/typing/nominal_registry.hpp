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
        std::optional<Template> tmpl;
        Specialization next_free = 0;
    };

    std::vector<Slot> slots_{ 1 }; // reserve 0 for invalid specialization
    Specialization free_head_ = 0;
    mutable std::mutex mutex_;

public:
    Specialization add(Template tmpl)
    {
        assert_valid(tmpl);

        std::unique_lock lock(mutex_);

        Specialization spec;

        if (free_head_ != 0)
        {
            spec = free_head_;
            free_head_ = slots_[spec].next_free;
        }
        else
        {
            spec = static_cast<Specialization>(slots_.size());
            slots_.emplace_back();
        }

        slots_[spec].tmpl.emplace(std::move(tmpl));
        slots_[spec].next_free = 0;

        return spec;
    }

    bool remove(Specialization spec)
    {
        std::unique_lock lock(mutex_);

        if (auto* slot = try_get_slot_unsafe(spec))
        {
            slot->tmpl.reset();
            slot->next_free = free_head_;
            free_head_ = spec;

            return true;
        }
        else
            return false;
    }

    bool exists(Specialization spec) const
    {
        std::unique_lock lock(mutex_);
        return try_get_slot_unsafe(spec) != nullptr;
    }

    const Template& get(Specialization spec) const
    {
        std::unique_lock lock(mutex_);
        return *get_slot_unsafe(spec).tmpl;
    }

protected:
    virtual void assert_valid(const Template& tmpl) const
    {
    }

private:
    const Slot* try_get_slot_unsafe(Specialization spec) const
    {
        if (spec == 0 || spec >= slots_.size() || !slots_[spec].tmpl)
            return nullptr;
        else
            return &slots_[spec];
    }

    Slot* try_get_slot_unsafe(Specialization spec)
    {
        if (spec == 0 || spec >= slots_.size() || !slots_[spec].tmpl)
            return nullptr;
        else
            return &slots_[spec];
    }

    const Slot& get_slot_unsafe(Specialization spec) const
    {
        if (auto* slot = try_get_slot_unsafe(spec))
            return *slot;
        else
            throw std::logic_error("Specialization does not exist");
    }
};

} // crucible::core::typing
