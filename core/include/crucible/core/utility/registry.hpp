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
private:
    class UnsafeIterator;

public:
    class Lock
    {
    public:
        const Registry& registry;

    private:
        std::unique_lock<std::mutex> lock_;

    public:
        Lock(const Registry& registry)
            : registry(registry)
            , lock_(registry.mutex_)
        {
        }

        void assert_valid(const Registry* registry) const
        {
            if (&this->registry != registry)
                throw std::runtime_error("Lock does not point to registry");
        }
    };

    class Range
    {
        friend class Registry;

        Lock lock_;

    private:
        Range(const Registry& registry)
            : lock_(registry)
        {
        }

    public:
        UnsafeIterator begin() const
        {
            return UnsafeIterator(lock_.registry, true);
        }

        UnsafeIterator end() const
        {
            return UnsafeIterator(lock_.registry, false);
        }
    };

    class SafeRange
    {
        friend class Registry;

        const Lock& lock_;

    private:
        SafeRange(const Lock& lock)
            : lock_(lock)
        {
        }

    public:
        UnsafeIterator begin() const
        {
            return UnsafeIterator(lock_.registry, true);
        }

        UnsafeIterator end() const
        {
            return UnsafeIterator(lock_.registry, false);
        }
    };

private:
    class UnsafeIterator
    {
    public:
        struct Entry
        {
            Handle handle;
            const Element& element;
        };

    private:
        const Registry& registry_;
        Handle handle_ = 0;
        Handle max_handle_ = 0;

    public:
        UnsafeIterator(const Registry& registry, bool at_start)
            : registry_(registry)
        {
            max_handle_ = static_cast<Handle>(
                (registry_.pages_.size() - 1) * Page::kSize + registry_.pages_.back()->filled);

            if (at_start)
                advance();
            else
                handle_ = max_handle_ + 1;
        }

        UnsafeIterator& operator++()
        {
            advance();
            return *this;
        }

        Entry operator*() const
        {
            return Entry{
                .handle = handle_,
                .element = registry_.get_unsafe(handle_)
            };
        }

        bool operator!=(const UnsafeIterator& o) const
        {
            return handle_ != o.handle_;
        }

    private:
        void advance()
        {
            while (handle_ <= max_handle_)
            {
                if (registry_.exists_unsafe(++handle_))
                    break;
            }
        }
    };

    struct Slot
    {
        std::optional<Element> el;
        Handle next_free = 0;
    };

    struct Page
    {
        constexpr static inline size_t kSize = 256;

        std::array<Slot, kSize> my_slots{};
        size_t filled = 0;
    };

protected:
    mutable std::mutex mutex_;

private:
    std::vector<std::unique_ptr<Page>> pages_;
    Handle free_head_ = 0;

public:
    Registry()
    {
        pages_.emplace_back(std::make_unique<Page>());
    }

    Lock lock() const
    {
        return Lock(*this);
    }

    Handle add(Element el)
    {
        std::unique_lock lock(mutex_);
        return add_unsafe(std::move(el));
    }

    Handle add(const Lock& lock, Element el)
    {
        lock.assert_valid(this);
        return add_unsafe(std::move(el));
    }

    bool remove(Handle handle)
    {
        std::unique_lock lock(mutex_);
        return remove_unsafe(handle);
    }

    bool remove(const Lock& lock, Handle handle)
    {
        lock.assert_valid(this);
        return remove_unsafe(handle);
    }

    bool exists(Handle handle) const
    {
        std::unique_lock lock(mutex_);
        return exists_unsafe(handle);
    }

    bool exists(const Lock& lock, Handle handle) const
    {
        lock.assert_valid(this);
        return exists_unsafe(handle);
    }

    // TODO might as well get rid of ThreadSafeReference/ThreadSafeValue as well as all methods that don't pass lock()

    ThreadSafeReference<const Element> get(Handle handle) const
    {
        std::unique_lock lock(mutex_);
        return ThreadSafeReference<const Element>(std::move(lock), get_unsafe(handle));
    }

    ThreadSafeReference<Element> get(Handle handle)
    {
        std::unique_lock lock(mutex_);
        return ThreadSafeReference<Element>(std::move(lock), get_unsafe(handle));
    }

    const Element& get(const Lock& lock, Handle handle) const
    {
        lock.assert_valid(this);
        return get_unsafe(handle);
    }

    Element& get(const Lock& lock, Handle handle)
    {
        lock.assert_valid(this);
        return get_unsafe(handle);
    }

    Range range() const
    {
        return Range(*this);
    }

    SafeRange range(const Lock& lock) const
    {
        lock.assert_valid(this);
        return SafeRange(lock);
    }

    ThreadSafeValue<Handle> peek_next_handle() const
    {
        std::unique_lock lock(mutex_);
        return ThreadSafeValue<Handle>(std::move(lock), peek_next_handle_unsafe());
    }

    Handle peek_next_handle(const Lock& lock) const
    {
        lock.assert_valid(this);
        return peek_next_handle_unsafe();
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
            free_head_ = pages_[page_index]->my_slots[slot_index].next_free;
        }
        else
        {
            if (pages_.back()->filled == Page::kSize)
                pages_.emplace_back(std::make_unique<Page>());

            Page& page = *pages_.back();
            handle = static_cast<Handle>((pages_.size() - 1) * Page::kSize + pages_.back()->filled + 1);
            ++page.filled;
        }

        size_t page_index, slot_index;
        get_indexes(handle, page_index, slot_index);
        Slot& slot = pages_[page_index]->my_slots[slot_index];
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

    Handle peek_next_handle_unsafe()
    {
        if (free_head_ != 0)
            return free_head_;
        else
        {
            size_t page_count = pages_.size();
            size_t last_page_filled = pages_.back()->filled;
            if (pages_.back()->filled == Page::kSize)
            {
                ++page_count;
                last_page_filled = 0;
            }

            return static_cast<Handle>((page_count - 1) * Page::kSize + last_page_filled + 1);
        }
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

        const Slot& slot = pages_[page_index]->my_slots[slot_index];
        return slot.el ? &slot : nullptr;
    }

    Slot* try_get_slot_unsafe(Handle handle)
    {
        size_t page_index, slot_index;
        if (!get_indexes(handle, page_index, slot_index))
            return nullptr;

        if (page_index >= pages_.size())
            return nullptr;

        Slot& slot = pages_[page_index]->my_slots[slot_index];
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
