#pragma once

#include <QObject>

namespace crucible::gui
{

class SignalPropertyBase
    : public QObject
{
    Q_OBJECT;

signals:
    void modified();
};

template<typename T>
class SignalProperty
    : public SignalPropertyBase
{
private:
    T value_;

public:
    SignalProperty(T default_value)
        : value_(std::move(default_value))
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

    const T& get() const
    {
        return value_;
    }

    void set(T value)
    {
        value_ = std::move(value);
        Q_EMIT modified();
    }
};

} // crucible::gui
