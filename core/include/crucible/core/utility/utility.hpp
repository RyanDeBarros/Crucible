#pragma once

#include <mutex>

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

} // crucible::core
