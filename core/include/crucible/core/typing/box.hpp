#pragma once

#include <crucible/core/typing/type_erasure.hpp>

#include <concepts>
#include <memory>
#include <stdexcept>

namespace crucible::core::typing
{

class Box
{
    using Dtor = void (*)(void*);
    using Copier = void* (*)(const void*);
    using CopyAssign = bool(*)(void*, const void*);
    using MoveAssign = bool(*)(void*, void*);

    struct Ops
    {
        TypeErasure type; // TODO flesh out imp::box with type and hasher in Ops
        Dtor dtor = nullptr;
        Copier copy = nullptr;
        CopyAssign copy_assign = nullptr;
        MoveAssign move_assign = nullptr;
    };

private:
    template<typename T>
    static constexpr Ops kOps = {
        .type = erase_type<T>(),
        .dtor = [](void* ptr) { delete static_cast<T*>(ptr); },
        .copy = [](const void* ptr) -> void* {
            if constexpr (std::is_copy_constructible_v<T>)
                return ptr ? new T(*static_cast<const T*>(ptr)) : nullptr;
            else
                return nullptr;
        },
        .copy_assign = [](void* to, const void* from) {
            if constexpr (std::is_copy_assignable_v<T>)
            {
                *static_cast<T*>(to) = *static_cast<const T*>(from);
                return true;
            }
            else
                return false;
        },
        .move_assign = [](void* to, void* from) {
            if constexpr (std::is_move_assignable_v<T>)
            {
                *static_cast<T*>(to) = std::move(*static_cast<T*>(from));
                return true;
            }
            else
                return false;
        }
    };

    void* raw_ = nullptr;
    const Ops* ops_ = nullptr;

public:
    Box() = default;

    template<typename T>
    Box(T* raw)
        : raw_(raw)
        , ops_(&kOps<T>)
    {
    }

    Box(const Box& o)
        : raw_(o.try_copy(o.raw_))
        , ops_(o.ops_)
    {
        if (o.raw_ && !raw_)
            throw std::logic_error("Attempted to copy a non-copyable type");
    }

    Box(Box&& o) noexcept
        : raw_(o.raw_)
        , ops_(o.ops_)
    {
        o.raw_ = nullptr;
        o.ops_ = nullptr;
    }

    ~Box()
    {
        try_dtor(raw_);
    }

    Box& operator=(const Box& o)
    {
        if (this != &o)
        {
            if (type() != o.type() || !try_copy_assign(raw_, o.raw_))
            {
                try_dtor(raw_);
                raw_ = o.try_copy(o.raw_);
            }

            type() = o.type();
            ops_ = o.ops_;

            if (o.raw_ && !raw_)
                throw std::logic_error("Attempted to copy a non-copyable type");
        }

        return *this;
    }

    Box& operator=(Box&& o) noexcept
    {
        if (this != &o)
        {
            if (type() != o.type() || !try_move_assign(raw_, o.raw_))
            {
                try_dtor(raw_);
                raw_ = o.raw_;
            }

            ops_ = o.ops_;

            o.raw_ = nullptr;
            o.ops_ = nullptr;
        }

        return *this;
    }

public:
    template<typename T>
    bool holds() const
    {
        return type() == erase_type<T>();
    }

    bool holds(TypeErasure type) const
    {
        return this->type() == type;
    }

    TypeErasure type() const
    {
        return ops_ ? ops_->type : TypeErasure();
    }

    const void* unsafe_raw() const
    {
        return raw_;
    }

    void* unsafe_raw()
    {
        return raw_;
    }

    template<typename T>
    const T* as() const
    {
        if (holds<T>())
            return static_cast<const T*>(raw_);
        else
            return nullptr;
    }

    template<typename T>
    T* as()
    {
        if (holds<T>())
            return static_cast<T*>(raw_);
        else
            return nullptr;
    }

    template<typename T>
    std::unique_ptr<T> consume()
    {
        if (holds<T>())
        {
            std::unique_ptr<T> ptr(static_cast<T*>(raw_));
            raw_ = nullptr;
            return ptr;
        }
        else
            return nullptr;
    }

private:
    void try_dtor(void* ptr) const
    {
        if (ops_)
            ops_->dtor(ptr);
    }

    void* try_copy(const void* ptr) const
    {
        if (ops_)
            return ops_->copy(ptr);
        else
            return nullptr;
    }

    bool try_copy_assign(void* to, const void* from)
    {
        if (to && from)
        {
            if (ops_)
                return ops_->copy_assign(to, from);
        }

        return false;
    }

    bool try_move_assign(void* to, void* from)
    {
        if (to && from)
        {
            if (ops_)
                return ops_->move_assign(to, from);
        }

        return false;
    }
};

template<typename T, typename... Args>
    requires (!std::is_void_v<T>)
Box make_box(Args&&... args)
{
    return Box(new T(std::forward<Args>(args)...));
}

template<typename T, typename... Args>
    requires (!std::is_void_v<T>)
void copy_box(Box& b, Args&&... args)
{
    if (T* obj = b.as<T>())
        *obj = T(std::forward<Args>(args)...);
    else
        b = make_box<T>(std::forward<Args>(args)...);
}

template<typename T>
Box forward_to_box(T&& obj)
{
    return make_box<std::remove_cvref_t<T>>(std::forward<T>(obj));
}

template<typename T>
void forward_into_box(Box& b, T&& obj)
{
    copy_box<std::remove_cvref_t<T>>(b, std::forward<T>(obj));
}

} // crucible::core::typing
