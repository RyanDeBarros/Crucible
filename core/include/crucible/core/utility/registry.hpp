#pragma once

#include <crucible/core/utility/utility.hpp>

#include <deque>
#include <optional>
#include <stdexcept>
#include <vector>

namespace crucible::core
{

template<typename Element, typename Handle = size_t>
class Registry
{
    // TODO use paged storage for stable addresses instead of std::deque to reduce allocations. For instance, each page can have 256 modules/templates, then store them in std::vector<std::unique_ptr<Page>>.

    struct Slot
    {
        std::optional<Element> el;
        Handle next_free = 0;
    };

private:
    std::deque<Slot> slots_{ 1 }; // reserve 0 for invalid handle
    Handle free_head_ = 0;

protected:
    mutable std::mutex mutex_;

public:
    Handle add(Element el)
    {
        std::unique_lock lock(mutex_);
        return add_unsafe(std::move(el));
    }

    bool remove(Handle handle)
    {
        std::unique_lock lock(mutex_);
        return remove_unsafe(handle);
    }

    bool exists(Handle handle) const
    {
        std::unique_lock lock(mutex_);
        return exists_unsafe(handle);
    }

    ThreadSafeReference<Element> get(Handle handle) const
    {
        std::unique_lock lock(mutex_);
        return ThreadSafeReference<Element>(std::move(lock), get_unsafe(handle));
    }

protected:
    Handle add_unsafe(Element el)
    {
        Handle handle;

        if (free_head_ != 0)
        {
            handle = free_head_;
            free_head_ = slots_[handle].next_free;
        }
        else
        {
            handle = static_cast<Handle>(slots_.size());
            slots_.emplace_back();
        }

        slots_[handle].el.emplace(std::move(el));
        slots_[handle].next_free = 0;

        return handle;
    }

    bool remove_unsafe(Handle handle)
    {
        if (auto* slot = try_get_slot_unsafe(handle))
        {
            slot->el.reset();
            slot->next_free = free_head_;
            free_head_ = handle;

            return true;
        }
        else
            return false;
    }

    bool exists_unsafe(Handle handle) const
    {
        return try_get_slot_unsafe(handle) != nullptr;
    }

    const Element& get_unsafe(Handle handle) const
    {
        return *get_slot_unsafe(handle).el;
    }

private:
    const Slot* try_get_slot_unsafe(Handle handle) const
    {
        if (handle == 0 || handle >= slots_.size() || !slots_[handle].el)
            return nullptr;
        else
            return &slots_[handle];
    }

    Slot* try_get_slot_unsafe(Handle handle)
    {
        if (handle == 0 || handle >= slots_.size() || !slots_[handle].el)
            return nullptr;
        else
            return &slots_[handle];
    }

    const Slot& get_slot_unsafe(Handle handle) const
    {
        if (auto* slot = try_get_slot_unsafe(handle))
            return *slot;
        else
            throw std::logic_error("Handle does not exist");
    }
};

} // crucible::core
