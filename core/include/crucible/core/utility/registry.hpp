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
    class Range;
    class UnsafeIterator;

public:
    class Lock
    {
        friend class Range;

    private:
        const Registry* registry_;
        std::unique_lock<std::mutex> lock_; // TODO allow read-only locks simultaneously on same thread or different threads

    public:
        Lock(const Registry& registry)
            : registry_(&registry)
            , lock_(registry.mutex_)
        {
        }

        Lock(const Lock&) = delete;

        Lock(Lock&& o) noexcept
            : registry_(o.registry_)
            , lock_(std::move(o.lock_))
        {
            if (registry_ && registry_->active_lock_ == &o)
                registry_->active_lock_ = this;
        }

        ~Lock()
        {
            if (registry_ && registry_->active_lock_ == this)
                registry_->active_lock_ = nullptr;
        }

        Lock& operator=(const Lock&) = delete;

        Lock& operator=(Lock&& o) noexcept
        {
            if (this != &o)
            {
                if (registry_ && registry_->active_lock_ == this)
                    registry_->active_lock_ = nullptr;

                registry_ = o.registry_;

                if (registry_ && registry_->active_lock_ == &o)
                    registry_->active_lock_ = this;

                lock_ = std::move(o.lock_);
            }

            return *this;
        }

        void assert_valid(const Registry* registry) const
        {
            if (registry_ == nullptr)
                throw std::runtime_error("Null pointer");

            if (registry_ != registry)
                throw std::runtime_error("Lock does not point to registry");

            if (!lock_.owns_lock())
                throw std::runtime_error("Lock does not own the mutex");
        }
    };

    class Range
    {
        friend class Registry;

        const Lock& lock_;

    private:
        Range(const Lock& lock)
            : lock_(lock)
        {
        }

    public:
        UnsafeIterator begin() const
        {
            lock_.assert_valid(lock_.registry_);
            return UnsafeIterator(*lock_.registry_, true);
        }

        UnsafeIterator end() const
        {
            lock_.assert_valid(lock_.registry_);
            return UnsafeIterator(*lock_.registry_, false);
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
    size_t count_ = 0;
    mutable Lock* active_lock_ = nullptr;

public:
    Registry()
    {
        pages_.emplace_back(std::make_unique<Page>());
    }

    Lock lock() const
    {
        if (active_lock_ == nullptr)
        {
            Lock lock(*this);
            active_lock_ = &lock;
            return lock;
        }
        else
            throw std::runtime_error("Active lock already exists on registry");
    }

    Handle add(const Lock& lock, Element el)
    {
        lock.assert_valid(this);
        return add_unsafe(std::move(el));
    }

    bool remove(const Lock& lock, Handle handle)
    {
        lock.assert_valid(this);
        return remove_unsafe(handle);
    }

    bool exists(const Lock& lock, Handle handle) const
    {
        lock.assert_valid(this);
        return exists_unsafe(handle);
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

    Range range(const Lock& lock) const
    {
        lock.assert_valid(this);
        return Range(lock);
    }

    size_t size(const Lock& lock) const
    {
        lock.assert_valid(this);
        return size_unsafe();
    }

    void clear(const Lock& lock)
    {
        lock.assert_valid(this);
        clear_unsafe();
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

        ++count_;
        return handle;
    }

    bool remove_unsafe(Handle handle)
    {
        if (auto* slot = try_get_slot_unsafe(handle))
        {
            slot->el.reset();
            slot->next_free = free_head_;
            free_head_ = handle;

            --count_;
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

    size_t size_unsafe() const
    {
        return count_;
    }

    void clear_unsafe()
    {
        free_head_ = 0;
        count_ = 0;

        for (size_t page_index = pages_.size(); page_index-- > 0;)
        {
            Page& page = *pages_[page_index];

            for (size_t slot_index = page.filled; slot_index-- > 0;)
            {
                Slot& slot = page.my_slots[slot_index];

                slot.el.reset();
                slot.next_free = free_head_;

                free_head_ = static_cast<Handle>(
                    page_index * Page::kSize + slot_index + 1);
            }
        }
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
