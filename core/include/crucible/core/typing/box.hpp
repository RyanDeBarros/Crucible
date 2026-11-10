#pragma once

#include <crucible/core/typing/type_erasure.hpp>

#include <concepts>
#include <memory>

namespace crucible::core::typing
{

class Box
{
    void* raw_ = nullptr;
    TypeErasure type_;

public:
    Box() = default;

    template<typename T>
    Box(T* raw)
        : raw_(raw)
        , type_(erase_type<T>())
    {
    }

    Box(const Box&);
    Box(Box&&) noexcept;
    ~Box();

    Box& operator=(const Box&);
    Box& operator=(Box&&) noexcept;

    bool operator==(const Box&) const;
    bool operator!=(const Box&) const;

public:
    template<typename T>
    bool holds() const
    {
        return holds(erase_type<T>());
    }

    bool holds(TypeErasure type) const;
    TypeErasure type() const;
    const void* unsafe_raw() const;
    void* unsafe_raw();

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

    size_t hash() const;

private:
    void try_dtor(void* ptr) const;
    void* try_copy(const void* ptr) const;
    bool try_copy_assign(void* to, const void* from);
    bool try_move_assign(void* to, void* from);
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

template<>
struct std::hash<crucible::core::typing::Box>
{
    size_t operator()(const crucible::core::typing::Box& box) const;
};
