#include <crucible/core/typing/storage.hpp>

#include <crucible/core/typing/registries.hpp>

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
    if (!is_valid_payload(type, payload))
        throw std::logic_error("Cannot initialize storage with invalid payload");

    return Storage(std::move(type), std::move(payload), StorageState::Initialized);
}

Storage Storage::make_null(QualifiedType type)
{
    return Storage(std::move(type), Box{}, StorageState::Null);
}

static bool validate_struct_fields(Specialization spec, const StructPayload& payload)
{
    const size_t field_count = gStructRegistry.struct_field_count(spec);
    if (field_count != payload.value.size())
        return false;

    for (size_t i = 0; i < field_count; ++i)
    {
        if (!Storage::is_valid_payload(
            gStructRegistry.struct_field_type(spec, i),
            payload.value[i]))
        {
            return false;
        }
    }

    return true;
}

bool Storage::is_valid_payload(const QualifiedType& type, const Box& payload)
{
    switch (type.base_type())
    {
    case Kind::Bool:
        return payload.holds<BoolPayload>();

    case Kind::Int:
        return payload.holds<IntPayload>();

    case Kind::Float:
        return payload.holds<FloatPayload>();

    case Kind::String:
        return payload.holds<StringPayload>();

    case Kind::Pointer:
        return payload.holds<PointerPayload>();

    case Kind::Enum:
        return payload.holds<EnumPayload>()
            && gEnumRegistry.exists(type.specialization());

    case Kind::List:
        // TODO
        break;
        //return payload.holds<ListPayload>();

    case Kind::Set:
        // TODO
        break;
        //return payload.holds<SetPayload>();

    case Kind::Map:
        // TODO
        break;
        //return payload.holds<MapPayload>();

    case Kind::Struct:
        return payload.holds<StructPayload>()
            && gStructRegistry.exists(type.specialization())
            && validate_struct_fields(type.specialization(), *payload.as<StructPayload>());

    case Kind::Joker:
        return payload.holds<JokerPayload>();
    }
    return false;
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
