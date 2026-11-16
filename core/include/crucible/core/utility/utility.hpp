#pragma once

#include <mutex>

namespace crucible
{

template<auto>
constexpr bool kFalse = false;

template<typename T>
class ThreadSafeValue
{
    std::unique_lock<std::mutex> lock_;
    T value_;

public:
    ThreadSafeValue(std::unique_lock<std::mutex>&& lock, T value)
        : lock_(std::move(lock))
        , value_(std::move(value))
    {
    }

    const T& operator*() const
    {
        return value_;
    }

    const T* operator->() const
    {
        return &value_;
    }
};

template<typename T>
class ThreadSafeReference
{
    std::unique_lock<std::mutex> lock_;
    T& ref_;

public:
    ThreadSafeReference(std::unique_lock<std::mutex>&& lock, T& ref)
        : lock_(std::move(lock))
        , ref_(ref)
    {
    }

    T& operator*() const
    {
        return ref_;
    }

    T* operator->() const
    {
        return &ref_;
    }
};

} // crucible::core
