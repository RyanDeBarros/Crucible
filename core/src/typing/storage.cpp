#include <crucible/core/typing/storage.hpp>

namespace crucible::core::typing
{

Storage::Storage(QualifiedType type, Box payload, StorageState state)
    : type_(std::move(type))
    , payload_(std::move(payload))
    , state_(state)
{
}

const QualifiedType& Storage::type() const
{
    return type_;
}

StorageState Storage::state() const
{
    return state_;
}

Storage Storage::make_uninitialized(QualifiedType type)
{
    return Storage(std::move(type), Box{}, StorageState::Uninitialized);
}

Storage Storage::make_initialized(QualifiedType type, Box payload)
{
    return Storage(std::move(type), std::move(payload), StorageState::Initialized);
}

Storage Storage::make_null(QualifiedType type)
{
    return Storage(std::move(type), Box{}, StorageState::Null);
}

const Box* Storage::get(const QualifiedType& type) const
{
    if (state_ != StorageState::Initialized)
        return nullptr;

    if (type != type_)
        return nullptr;

    return &payload_;
}

Box* Storage::get(const QualifiedType& type)
{
    if (state_ != StorageState::Initialized)
        return nullptr;

    if (type != type_)
        return nullptr;

    return &payload_;
}

bool Storage::set(Storage other)
{
    if (type_ != other.type_)
        return false;

    payload_ = std::move(other.payload_);
    state_ = other.state_;
    return true;
}

} // crucible::core::typing
