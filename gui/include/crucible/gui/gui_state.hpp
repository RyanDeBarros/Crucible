#pragma once

#include <QSettings>
#include <QVariant>

namespace crucible::gui
{

struct GuiState
    : public QSettings
{
    template<typename T>
    class Value
    {
        friend struct GuiState;

    private:
        GuiState& state_;
        QString key_;
        T value_;

    private:
        Value(GuiState& state, QAnyStringView key, T default_value)
            : state_(state)
            , key_(key.toString())
            , value_(state.value(key, default_value).value<T>())
        {
        }

    public:
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
            state_.setValue(key_, std::move(value));
        }
    };

public:
    GuiState();

    const class Logging
    {
        GuiState& state_;

    public:
        Logging(GuiState& state);

        Value<bool> word_wrap() const;
    } logging;
};

} // crucible::gui
