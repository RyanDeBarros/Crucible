#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace crucible::core::typing
{

constexpr uint64_t fnv1a_hash(std::string_view str) noexcept
{
    uint64_t hash = 0xcbf29ce484222325ULL;
    for (char c : str)
    {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
        hash *= 0x00000100000001B3ULL;
    }
    return hash;
}

template<typename T>
constexpr std::string_view compiler_type_name() noexcept
{
#if defined(_MSC_VER)
    return __FUNCSIG__;
#else
    return __PRETTY_FUNCTION__;
#endif
}

template<typename T>
constexpr uint64_t type_id() noexcept
{
    return fnv1a_hash(compiler_type_name<std::remove_cvref_t<T>>());
}

struct TypeErasure
{
    template<typename T>
    friend constexpr TypeErasure erase_type() noexcept;

private:
    uint64_t uid_;

private:
    constexpr explicit TypeErasure(uint64_t uid) noexcept
        : uid_(uid)
    {
    }

public:
    constexpr TypeErasure() noexcept
        : uid_(type_id<void>())
    {
    }

    constexpr bool operator==(const TypeErasure&) const noexcept = default;
    constexpr bool operator!=(const TypeErasure&) const noexcept = default;

    [[nodiscard]]
    constexpr size_t hash() const noexcept
    {
        return static_cast<size_t>(uid_);
    }

    [[nodiscard]]
    constexpr uint64_t uid() const
    {
        return uid_;
    }
};

template<typename T>
constexpr TypeErasure erase_type() noexcept
{
    return TypeErasure(type_id<T>());
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
