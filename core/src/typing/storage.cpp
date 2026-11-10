#include <crucible/core/typing/storage.hpp>

#include <crucible/core/typing/intern_registries.hpp>
#include <crucible/core/typing/nominal_registries.hpp>

namespace crucible::core::typing
{

Storage::Storage(QualifiedType type, reflector::box payload, StorageState state)
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
    return Storage(std::move(type), reflector::box{}, StorageState::Uninitialized);
}

Storage Storage::make_initialized(QualifiedType type, reflector::box payload)
{
    if (!is_valid_payload(type, payload))
        throw std::logic_error("Cannot initialize storage with invalid payload");

    return Storage(std::move(type), std::move(payload), StorageState::Initialized);
}

Storage Storage::make_null(QualifiedType type)
{
    return Storage(std::move(type), reflector::box{}, StorageState::Null);
}

static bool validate_pointer(Specialization spec, const PointerPayload& payload)
{
    if (!gPointerRegistry.exists(spec))
        return false;

    const auto underlying = gPointerRegistry.underlying_type(spec);
    // TODO return gModuleRegistry.type(payload.value) == underlying;
    return true;
}

static bool validate_enum(Specialization spec, const EnumPayload& payload)
{
    if (!gEnumRegistry.exists(spec))
        return false;
    
    return payload.value < gEnumRegistry.enum_count(spec);
}

static bool validate_list(Specialization spec, const ListPayload& payload)
{
    if (!gListRegistry.exists(spec))
        return false;

    const auto element_type = gListRegistry.element_type(spec);
    for (const auto& elem : payload.value)
    {
        if (!Storage::is_valid_payload(element_type, elem))
            return false;
    }

    return true;
}

static bool validate_set(Specialization spec, const SetPayload& payload)
{
    if (!gSetRegistry.exists(spec))
        return false;

    const auto element_type = gSetRegistry.element_type(spec);
    for (const auto& elem : payload.value)
    {
        if (!Storage::is_valid_payload(element_type, elem))
            return false;
    }

    return true;
}

static bool validate_map(Specialization spec, const MapPayload& payload)
{
    if (!gMapRegistry.exists(spec))
        return false;

    const auto key_type = gMapRegistry.key_type(spec);
    const auto value_type = gMapRegistry.value_type(spec);
    for (const auto& [key, value] : payload.value)
    {
        if (!Storage::is_valid_payload(key_type, key) ||
            !Storage::is_valid_payload(value_type, value))
        {
            return false;
        }
    }

    return true;
}

static bool validate_struct(Specialization spec, const StructPayload& payload)
{
    if (!gStructRegistry.exists(spec))
        return false;

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

// TODO validation check is long and should already be done in GUI - don't call in ctor/setter
bool Storage::is_valid_payload(const QualifiedType& type, const reflector::box& payload)
{
    switch (type.kind())
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
        if (auto* pld = payload.as<PointerPayload>())
            return validate_pointer(type.specialization(), *pld);
        else
            return false;

    case Kind::Enum:
        if (auto* pld = payload.as<EnumPayload>())
            return validate_enum(type.specialization(), *pld);
        else
            return false;

    case Kind::List:
        if (auto* pld = payload.as<ListPayload>())
            return validate_list(type.specialization(), *pld);
        else
            return false;

    case Kind::Set:
        if (auto* pld = payload.as<SetPayload>())
            return validate_set(type.specialization(), *pld);
        else
            return false;

    case Kind::Map:
        if (auto* pld = payload.as<MapPayload>())
            return validate_map(type.specialization(), *pld);
        else
            return false;

    case Kind::Struct:
    {
        if (const auto* pld = payload.as<StructPayload>())
            return validate_struct(type.specialization(), *pld);
        else
            return false;
    }

    case Kind::Joker:
        return payload.holds<JokerPayload>();
    }
    return false;
}

const reflector::box* Storage::get(const QualifiedType& type) const
{
    if (state_ != StorageState::Initialized)
        return nullptr;

    if (type != type_)
        return nullptr;

    return &payload_;
}

reflector::box* Storage::get(const QualifiedType& type)
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
