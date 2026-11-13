#pragma once

#include <mutex>
#include <stdexcept>

namespace crucible::core
{

template<auto>
constexpr bool kFalse = false;

template<typename T>
class ThreadSafeReference
{
    std::unique_lock<std::mutex> lock_;
    const T& ref_;

public:
    ThreadSafeReference(std::unique_lock<std::mutex>&& lock, const T& ref)
        : lock_(std::move(lock))
        , ref_(ref)
    {
    }

    const T& operator*() const
    {
        return ref_;
    }

    const T* operator->() const
    {
        return &ref_;
    }
};

template<typename T>
class SafePointer
{
    T* ptr_ = nullptr;

public:
    SafePointer() = default;
    SafePointer(const SafePointer&) = default;
    SafePointer(SafePointer&&) noexcept = default;

    SafePointer(T* ptr)
        : ptr_(ptr)
    {
    }

    SafePointer& operator=(const SafePointer&) = default;
    SafePointer& operator=(SafePointer&&) noexcept = default;

    SafePointer& operator=(T* ptr)
    {
        ptr_ = ptr;
        return *this;
    }

    operator T* () const
    {
        return ptr_;
    }

    operator bool() const
    {
        return ptr_ != nullptr;
    }

    T& operator*() const
    {
        if (ptr_)
            return *ptr_;
        else
            throw std::runtime_error("Tried to dereference null pointer");
    }

    T* operator->() const
    {
        if (ptr_)
            return ptr_;
        else
            throw std::runtime_error("Tried to dereference null pointer");
    }
};

} // crucible::core
