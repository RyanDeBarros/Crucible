#pragma once

#include <mutex>

namespace crucible::core
{

template<auto>
constexpr bool kFalse = false;

template<typename Element>
class ThreadSafeReference
{
    std::unique_lock<std::mutex> lock_;
    const Element& element_;

public:
    ThreadSafeReference(std::unique_lock<std::mutex>&& lock, const Element& element)
        : lock_(std::move(lock))
        , element_(element)
    {
    }

    const Element& operator*() const
    {
        return element_;
    }

    const Element* operator->() const
    {
        return &element_;
    }
};

} // crucible::core
