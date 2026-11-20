#include <crucible/core/typing/storage.hpp>

#include <crucible/core/typing/intern_registries.hpp>
#include <crucible/core/typing/nominal_registries.hpp>

#include <crucible/core/modules/module_registry.hpp>

#include <crucible/core/utility/utility.hpp>

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
    return Storage(std::move(type), std::move(payload), StorageState::Initialized);
}

Storage Storage::make_null(QualifiedType type)
{
    return Storage(std::move(type), reflector::box{}, StorageState::Null);
}

struct RegistryLockPack
{
    modules::ModuleRegistry::Lock module_lock;
    PointerRegistry::Lock pointer_lock;
    EnumRegistry::Lock enum_lock;
    ListRegistry::Lock list_lock;
    SetRegistry::Lock set_lock;
    MapRegistry::Lock map_lock;
    StructRegistry::Lock struct_lock;

    RegistryLockPack()
        : module_lock(modules::gModuleRegistry.lock())
        , pointer_lock(gPointerRegistry.lock())
        , enum_lock(gEnumRegistry.lock())
        , list_lock(gListRegistry.lock())
        , set_lock(gSetRegistry.lock())
        , map_lock(gMapRegistry.lock())
        , struct_lock(gStructRegistry.lock())
    {
    }
};

static bool validate_pointer(const RegistryLockPack& locks, Specialization spec, const PointerPayload& payload);
static bool validate_enum(const RegistryLockPack& locks, Specialization spec, const EnumPayload& payload);
static bool validate_list(const RegistryLockPack& locks, Specialization spec, const ListPayload& payload);
static bool validate_set(const RegistryLockPack& locks, Specialization spec, const SetPayload& payload);
static bool validate_map(const RegistryLockPack& locks, Specialization spec, const MapPayload& payload);
static bool validate_struct(const RegistryLockPack& locks, Specialization spec, const StructPayload& payload);

template<typename PayloadType>
constexpr bool kShouldItercheck =
    std::is_same_v<PayloadType, PointerPayload> ||
    std::is_same_v<PayloadType, EnumPayload> ||
    std::is_same_v<PayloadType, ListPayload> ||
    std::is_same_v<PayloadType, SetPayload> ||
    std::is_same_v<PayloadType, MapPayload> ||
    std::is_same_v<PayloadType, StructPayload>;

template<typename PayloadType>
static bool validate_element(const RegistryLockPack& locks, Specialization spec, const PayloadType& payload)
{
    if constexpr (std::is_same_v<PayloadType, PointerPayload>)
        return validate_pointer(locks, spec, payload);

    else if constexpr (std::is_same_v<PayloadType, EnumPayload>)
        return validate_enum(locks, spec, payload);

    else if constexpr (std::is_same_v<PayloadType, ListPayload>)
        return validate_list(locks, spec, payload);

    else if constexpr (std::is_same_v<PayloadType, SetPayload>)
        return validate_set(locks, spec, payload);

    else if constexpr (std::is_same_v<PayloadType, MapPayload>)
        return validate_map(locks, spec, payload);

    else if constexpr (std::is_same_v<PayloadType, StructPayload>)
        return validate_struct(locks, spec, payload);

    else
        static_assert(kFalse<PayloadType>);
}

static bool validate_pointer(const RegistryLockPack& locks, Specialization spec, const PointerPayload& payload)
{
    if (!gPointerRegistry.exists(locks.pointer_lock, spec))
        return false;

    const auto expected_underlying_type = gPointerRegistry.underlying_type(locks.pointer_lock, spec);
    return modules::gModuleRegistry.module_type(locks.module_lock, payload.value) == expected_underlying_type;
}

static bool validate_enum(const RegistryLockPack& locks, Specialization spec, const EnumPayload& payload)
{
    if (!gEnumRegistry.exists(locks.enum_lock, spec))
        return false;
    
    return payload.value < gEnumRegistry.enum_count(locks.enum_lock, spec);
}

template<typename ElementPayloadType, typename PayloadContainer>
bool std_itercheck(const RegistryLockPack& locks, const auto& payload, const QualifiedType& expected_element_type)
{
    if (const auto* pld = payload.value.as<PayloadContainer>())
    {
        if constexpr (kShouldItercheck<ElementPayloadType>)
        {
            for (const auto& el : *pld)
            {
                if (!validate_element(locks, expected_element_type.specialization(), el))
                    return false;
            }
        }
        
        return true;
    }
    else
        return false;
}

#define EXPAND_MACRO_0(M) \
        M(Bool) \
        M(Int) \
        M(Float) \
        M(String) \
        M(Pointer) \
        M(Enum) \
        M(List) \
        M(Set) \
        M(Map) \
        M(Struct) \
        M(Joker)

#define EXPAND_MACRO_1(M, Arg) \
        M(Arg, Bool) \
        M(Arg, Int) \
        M(Arg, Float) \
        M(Arg, String) \
        M(Arg, Pointer) \
        M(Arg, Enum) \
        M(Arg, List) \
        M(Arg, Set) \
        M(Arg, Map) \
        M(Arg, Struct) \
        M(Arg, Joker)

#define ITERCHECK(Container, ElementType) \
        case Kind::ElementType: return std_itercheck<ElementType##Payload, Container<ElementType##Payload>>( \
            locks, payload, expected_element_type);

#define VALIDATE_ITERABLE(Container) switch (expected_element_type.kind()) { EXPAND_MACRO_1(ITERCHECK, Container) }

static bool validate_list(const RegistryLockPack& locks, Specialization spec, const ListPayload& payload)
{
    if (!gListRegistry.exists(locks.list_lock, spec))
        return false;

    const auto expected_element_type = gListRegistry.element_type(locks.list_lock, spec);
    VALIDATE_ITERABLE(std::vector);
    return false;
}

static bool validate_set(const RegistryLockPack& locks, Specialization spec, const SetPayload& payload)
{
    if (!gSetRegistry.exists(locks.set_lock, spec))
        return false;

    const auto expected_element_type = gSetRegistry.element_type(locks.set_lock, spec);
    VALIDATE_ITERABLE(std::unordered_set);
    return false;
}

#undef VALIDATE_ITERABLE
#undef ITERCHECK

template<typename KeyPayloadType, typename ValuePayloadType, typename PayloadContainer>
bool map_itercheck(
    const RegistryLockPack& locks,
    const PayloadContainer& payload,
    const QualifiedType& expected_key_type,
    const QualifiedType& expected_value_type)
{
    if (const auto* pld = payload.value.as<std::unordered_map<KeyPayloadType, ValuePayloadType>>())
    {
        if constexpr (kShouldItercheck<KeyPayloadType>)
        {
            for (const auto& [key, _] : *pld)
            {
                if (!validate_element(locks, expected_key_type.specialization(), key))
                    return false;
            }
        }

        if constexpr (kShouldItercheck<ValuePayloadType>)
        {
            for (const auto& [_, value] : *pld)
            {
                if (!validate_element(locks, expected_value_type.specialization(), value))
                    return false;
            }
        }

        return true;
    }
    else
        return false;
}

#define MAP_ITERCHECK(KeyPayloadType, ValueType) \
        case Kind::ValueType: return map_itercheck<KeyPayloadType, ValueType##Payload>( \
            locks, payload, expected_key_type, expected_value_type);

#define VALIDATE_MAP_INNER(KeyType) \
        case Kind::KeyType: switch (expected_value_type.kind()) { EXPAND_MACRO_1(MAP_ITERCHECK, KeyType##Payload) }

#define VALIDATE_MAP_OUTER switch (expected_key_type.kind()) { EXPAND_MACRO_0(VALIDATE_MAP_INNER) }

static bool validate_map(const RegistryLockPack& locks, Specialization spec, const MapPayload& payload)
{
    if (!gMapRegistry.exists(locks.map_lock, spec))
        return false;

    const auto expected_key_type = gMapRegistry.key_type(locks.map_lock, spec);
    const auto expected_value_type = gMapRegistry.value_type(locks.map_lock, spec);
    VALIDATE_MAP_OUTER;
    return false;
}

#undef VALIDATE_MAP_OUTER
#undef VALIDATE_MAP_INNER
#undef MAP_ITERCHECK

#undef EXPAND_MACRO_0
#undef EXPAND_MACRO_1

static bool validate_struct(const RegistryLockPack& locks, Specialization spec, const StructPayload& payload)
{
    if (!gStructRegistry.exists(locks.struct_lock, spec))
        return false;

    const size_t field_count = gStructRegistry.struct_field_count(locks.struct_lock, spec);
    if (field_count != payload.value.size())
        return false;

    for (size_t i = 0; i < field_count; ++i)
    {
        if (!Storage::is_valid_payload(
            gStructRegistry.struct_field_type(locks.struct_lock, spec, i),
            payload.value[i]))
        {
            return false;
        }
    }

    return true;
}

bool Storage::is_valid_payload(const QualifiedType& type, const reflector::box& payload)
{
    RegistryLockPack locks;

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
            return validate_pointer(locks, type.specialization(), *pld);
        else
            return false;

    case Kind::Enum:
        if (auto* pld = payload.as<EnumPayload>())
            return validate_enum(locks, type.specialization(), *pld);
        else
            return false;

    case Kind::List:
        if (auto* pld = payload.as<ListPayload>())
            return validate_list(locks, type.specialization(), *pld);
        else
            return false;

    case Kind::Set:
        if (auto* pld = payload.as<SetPayload>())
            return validate_set(locks, type.specialization(), *pld);
        else
            return false;

    case Kind::Map:
        if (auto* pld = payload.as<MapPayload>())
            return validate_map(locks, type.specialization(), *pld);
        else
            return false;

    case Kind::Struct:
    {
        if (const auto* pld = payload.as<StructPayload>())
            return validate_struct(locks, type.specialization(), *pld);
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

size_t std::hash<crucible::core::typing::BoolPayload>::operator()(
    const crucible::core::typing::BoolPayload& pld) const
{
    return std::hash<bool>{}(pld.value);
}

size_t std::hash<crucible::core::typing::IntPayload>::operator()(
    const crucible::core::typing::IntPayload& pld) const
{
    return std::hash<std::int64_t>{}(pld.value);
}

size_t std::hash<crucible::core::typing::FloatPayload>::operator()(
    const crucible::core::typing::FloatPayload& pld) const
{
    return std::hash<double>{}(pld.value);
}

size_t std::hash<crucible::core::typing::StringPayload>::operator()(
    const crucible::core::typing::StringPayload& pld) const
{
    return std::hash<std::string>{}(pld.value);
}

size_t std::hash<crucible::core::typing::PointerPayload>::operator()(
    const crucible::core::typing::PointerPayload& pld) const
{
    return std::hash<crucible::core::modules::ModuleId>{}(pld.value);
}

size_t std::hash<crucible::core::typing::EnumPayload>::operator()(
    const crucible::core::typing::EnumPayload& pld) const
{
    return std::hash<std::uint64_t>{}(pld.value);
}

size_t std::hash<crucible::core::typing::ListPayload>::operator()(
    const crucible::core::typing::ListPayload& pld) const
{
    return std::hash<reflector::box>{}(pld.value);
}

size_t std::hash<crucible::core::typing::SetPayload>::operator()(
    const crucible::core::typing::SetPayload& pld) const
{
    return std::hash<reflector::box>{}(pld.value);
}

size_t std::hash<crucible::core::typing::MapPayload>::operator()(
    const crucible::core::typing::MapPayload& pld) const
{
    return std::hash<reflector::box>{}(pld.value);
}

size_t std::hash<crucible::core::typing::StructPayload>::operator()(
    const crucible::core::typing::StructPayload& pld) const
{
    size_t h = 0;
    for (const auto& field : pld.value)
        h ^= std::hash<reflector::box>{}(field)+ 0x9e3779b9 + (h << 6) + (h >> 2);
    return h;
}

size_t std::hash<crucible::core::typing::JokerPayload>::operator()(
    const crucible::core::typing::JokerPayload& pld) const
{
    return std::hash<reflector::box>{}(pld.value);
}
