#pragma once

#include <optional>

namespace crucible
{

template<typename T>
class OptionalCopy
{
    const T& original_;
    std::optional<T> modified_;

public:
    explicit OptionalCopy(const T& original)
        : original_(original)
    {
    }

    T& modify()
    {
        if (!modified_)
            modified_.emplace(original_);

        return *modified_;
    }

    const T& get() const
    {
        return modified_ ? *modified_ : original_;
    }

    const T& operator*() const
    {
        return get();
    }

    const T* operator->() const
    {
        return &get();
    }
};

}
