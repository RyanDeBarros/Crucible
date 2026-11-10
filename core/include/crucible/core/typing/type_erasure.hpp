#pragma once

#include <crucible/core/typing/compiler_type_name.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

namespace crucible::core::typing
{

constexpr std::uint64_t fnv1a_hash(std::string_view str) noexcept
{
    std::uint64_t hash = 0xcbf29ce484222325ULL;
    for (char c : str)
    {
        hash ^= static_cast<std::uint64_t>(static_cast<unsigned char>(c));
        hash *= 0x00000100000001B3ULL;
    }
    return hash;
}

class Box;

struct TypeErasure
{
    template<typename T>
    friend constexpr TypeErasure erase_type() noexcept;

    struct Descriptor
    {
        friend class Box;
        friend class TypeErasure;

    public:
        std::string_view name;
        size_t type_hash;

    private:
        void (*dtor_op_)(void*);
        void* (*copy_op_)(const void*);
        void (*copy_assign_op_)(void*, const void*);
        void (*move_assign_op_)(void*, void*);
        size_t (*hasher_op_)(const void*);
        bool (*equal_op_)(const void*, const void*);
        bool (*not_equal_op_)(const void*, const void*);

    private:
        constexpr Descriptor(
            std::string_view name,
            size_t type_hash,
            void (*dtor_op)(void*),
            void* (*copy_op)(const void*),
            void (*copy_assign_op)(void*, const void*),
            void (*move_assign_op)(void*, void*),
            size_t(*hasher_op)(const void*),
            bool (*equal_op)(const void*, const void*),
            bool (*not_equal_op)(const void*, const void*)
        ) noexcept
            : name(name),
            type_hash(type_hash),
            dtor_op_(dtor_op),
            copy_op_(copy_op),
            copy_assign_op_(copy_assign_op),
            move_assign_op_(move_assign_op),
            hasher_op_(hasher_op),
            equal_op_(equal_op),
            not_equal_op_(not_equal_op)
        {
        }
    };

private:
    template<typename T>
    static constexpr Descriptor kDescriptor = Descriptor(
        compiler_type_name<T>(),
        fnv1a_hash(compiler_type_name<T>()),
        []() {
            if constexpr (!std::is_void_v<T>)
            {
                return +[](void* ptr) {
                    delete static_cast<T*>(ptr);
                };
            }
            else
                return nullptr;
        }(),
        []() {
            if constexpr (!std::is_void_v<T> && std::is_copy_constructible_v<T>)
            {
                return +[](const void* ptr) -> void* {
                    return ptr ? new T(*static_cast<const T*>(ptr)) : nullptr;
                };
            }
            else
                return nullptr;
        }(),
        []() {
            if constexpr (!std::is_void_v<T> && std::is_copy_assignable_v<T>)
            {
                return +[](void* to, const void* from) {
                    *static_cast<T*>(to) = *static_cast<const T*>(from);
                };
            }
            else
                return nullptr;
        }(),
        []() {
            if constexpr (!std::is_void_v<T> && std::is_move_assignable_v<T>)
            {
                return +[](void* to, void* from) {
                    *static_cast<T*>(to) = std::move(*static_cast<T*>(from));
                };
            }
            else
                return nullptr;
        }(),
        []() {
            if constexpr (!std::is_void_v<T> && requires(const T& value)
                { { std::hash<T>{}(value) } -> std::convertible_to<size_t>; })
            {
                return +[](const void* ptr) {
                    return std::hash<T>{}(*static_cast<const T*>(ptr));
                };
            }
            else
                return nullptr;
        }(),
        []() {
            if constexpr (!std::is_void_v<T> && requires(const T& a, const T& b)
                { { a == b } -> std::convertible_to<bool>; })
            {
                return +[](const void* lhs, const void* rhs) -> bool {
                    return *static_cast<const T*>(lhs) == *static_cast<const T*>(rhs);
                };
            }
            else
                return nullptr;
        }(),
        []() {
            if constexpr (!std::is_void_v<T> && requires(const T& a, const T& b)
                { { a != b } -> std::convertible_to<bool>; })
            {
                return +[](const void* lhs, const void* rhs) -> bool {
                    return *static_cast<const T*>(lhs) != *static_cast<const T*>(rhs);
                };
            }
            else
                return nullptr;
        }()
    );

    const Descriptor* desc_ = nullptr;

    constexpr explicit TypeErasure(const Descriptor* desc) noexcept
        : desc_(desc)
    {
    }

public:
    constexpr TypeErasure() noexcept
        : desc_(&kDescriptor<void>)
    {
    }

    constexpr bool operator==(const TypeErasure& o) const noexcept
    {
        return desc_ == o.desc_;
    }

    constexpr bool operator!=(const TypeErasure& o) const noexcept
    {
        return desc_ != o.desc_;
    }

    [[nodiscard]]
    constexpr size_t hash() const noexcept
    {
        return desc_->type_hash;
    }

    [[nodiscard]]
    constexpr std::string_view name() const noexcept
    {
        return desc_->name;
    }

    [[nodiscard]]
    constexpr const Descriptor& desc() const noexcept
    {
        return *desc_;
    }
};

template<typename T>
constexpr TypeErasure erase_type() noexcept
{
    return TypeErasure(&TypeErasure::kDescriptor<T>);
}

template<typename T, typename U>
constexpr bool erases_to() noexcept
{
    return erase_type<T>() == erase_type<U>();
}

template<typename T>
constexpr const T* resolve_type(TypeErasure erasure, const void* ptr) noexcept
{
    return erase_type<T>() == erasure ? static_cast<const T*>(ptr) : nullptr;
}

template<typename T>
constexpr T* resolve_type(TypeErasure erasure, void* ptr) noexcept
{
    return erase_type<T>() == erasure ? static_cast<T*>(ptr) : nullptr;
}

template<typename T>
constexpr const T* resolve_type(TypeErasure erasure, const T* ptr) noexcept
{
    return resolve_type<T>(erasure, static_cast<const void*>(ptr));
}

template<typename T>
constexpr T* resolve_type(TypeErasure erasure, T* ptr) noexcept
{
    return resolve_type<T>(erasure, static_cast<void*>(ptr));
}

} // crucible::core::typing

template<>
struct std::hash<crucible::core::typing::TypeErasure>
{
    size_t operator()(crucible::core::typing::TypeErasure te) const
    {
        return te.hash();
    }
};
