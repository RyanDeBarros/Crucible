#pragma once

#include <crucible/core/utility/utility.hpp>

#include <array>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

namespace crucible::core
{

template<typename Element, typename Handle = size_t>
class Registry
{
    struct Slot
    {
        std::optional<Element> el;
        Handle next_free = 0;
    };

    struct Page
    {
        constexpr static inline size_t kSize = 256;

        std::array<Slot, kSize> slots;
        size_t filled = 0;
    };

private:
    std::vector<std::unique_ptr<Page>> pages_;
    Handle free_head_ = 0;

protected:
    mutable std::mutex mutex_;

public:
    Registry()
    {
        pages_.emplace_back(std::make_unique<Page>());
    }

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
            size_t page_index, slot_index;
            get_indexes(handle, page_index, slot_index);
            free_head_ = pages_[page_index]->slots[slot_index].next_free;
        }
        else
        {
            const size_t page_index = pages_.size() - 1;
            if (pages_.back()->filled == Page::kSize)
                pages_.emplace_back(std::make_unique<Page>());

            Page& page = *pages_.back();
            handle = static_cast<Handle>((pages_.size() - 1) * Page::kSize + pages_.back()->filled + 1);
            ++page.filled;
        }

        size_t page_index, slot_index;
        get_indexes(handle, page_index, slot_index);
        Slot& slot = pages_[page_index]->slots[slot_index];
        slot.el.emplace(std::move(el));
        slot.next_free = 0;

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

    Element& get_unsafe(Handle handle)
    {
        return *get_slot_unsafe(handle).el;
    }

private:
    bool get_indexes(Handle handle, size_t& page_index, size_t& slot_index) const
    {
        if (handle == 0)
            return false;

        --handle;
        page_index = static_cast<size_t>(handle / Page::kSize);
        slot_index = handle - page_index * Page::kSize;
        return true;
    }

    const Slot* try_get_slot_unsafe(Handle handle) const
    {
        size_t page_index, slot_index;
        if (!get_indexes(handle, page_index, slot_index))
            return nullptr;

        if (page_index >= pages_.size())
            return nullptr;

        const Slot& slot = pages_[page_index]->slots[slot_index];
        return slot.el ? &slot : nullptr;
    }

    Slot* try_get_slot_unsafe(Handle handle)
    {
        size_t page_index, slot_index;
        if (!get_indexes(handle, page_index, slot_index))
            return nullptr;

        if (page_index >= pages_.size())
            return nullptr;

        Slot& slot = pages_[page_index]->slots[slot_index];
        return slot.el ? &slot : nullptr;
    }

    const Slot& get_slot_unsafe(Handle handle) const
    {
        if (auto* slot = try_get_slot_unsafe(handle))
            return *slot;
        else
            throw std::logic_error("Handle does not exist");
    }

    Slot& get_slot_unsafe(Handle handle)
    {
        if (auto* slot = try_get_slot_unsafe(handle))
            return *slot;
        else
            throw std::logic_error("Handle does not exist");
    }
};

} // crucible::core
