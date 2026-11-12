#pragma once

#include <crucible/core/modules/module_id.hpp>

#include <crucible/core/typing/qualified_type.hpp>

#include <external/reflector.hpp>

#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace crucible::core::typing
{

struct BoolPayload
{
    bool value;

    bool operator==(const BoolPayload&) const = default;
};

struct IntPayload
{
    std::int64_t value;

    bool operator==(const IntPayload&) const = default;
};

struct FloatPayload
{
    double value;

    bool operator==(const FloatPayload&) const = default;
};

struct StringPayload
{
    std::string value;

    bool operator==(const StringPayload&) const = default;
};

struct PointerPayload
{
    modules::ModuleId value;

    bool operator==(const PointerPayload&) const = default;
};

struct EnumPayload
{
    std::uint64_t value;

    bool operator==(const EnumPayload&) const = default;
};

struct ListPayload
{
    reflector::box value;

    template<typename T> // must be payload
    ListPayload(std::vector<T> value)
        : value(reflector::forward_to_box(std::move(value)))
    {
    }

    bool operator==(const ListPayload&) const = default;
};

struct SetPayload
{
    reflector::box value;

    template<typename T> // must be payload
    SetPayload(std::unordered_set<T> value)
        : value(reflector::forward_to_box(std::move(value)))
    {
    }

    bool operator==(const SetPayload&) const = default;
};

struct MapPayload
{
    reflector::box value;

    template<typename K, typename V> // must be payloads
    MapPayload(std::unordered_map<K, V> value)
        : value(reflector::forward_to_box(std::move(value)))
    {
    }

    bool operator==(const MapPayload&) const = default;
};

struct StructPayload
{
    std::vector<reflector::box> value; // must contain payloads

    bool operator==(const StructPayload&) const = default;
};

struct JokerPayload
{
    reflector::box value; // must contain a payload

    bool operator==(const JokerPayload&) const = default;
};

enum class StorageState
{
    Uninitialized,
    Initialized,
    Null
};

class Storage
{
    QualifiedType type_;
    reflector::box payload_;
    StorageState state_;

    Storage(QualifiedType type, reflector::box payload, StorageState state);

public:
    const QualifiedType& type() const;
    StorageState state() const;

    static Storage make_uninitialized(QualifiedType type);
    static Storage make_initialized(QualifiedType type, reflector::box payload);
    static Storage make_null(QualifiedType type);

    static bool is_valid_payload(const QualifiedType& type, const reflector::box& payload);

    const reflector::box* get(const QualifiedType& type) const;
    reflector::box* get(const QualifiedType& type);
    bool set(Storage other);
};

} // crucible::core::typing

template<>
struct std::hash<crucible::core::typing::BoolPayload>
{
    size_t operator()(const crucible::core::typing::BoolPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::IntPayload>
{
    size_t operator()(const crucible::core::typing::IntPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::FloatPayload>
{
    size_t operator()(const crucible::core::typing::FloatPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::StringPayload>
{
    size_t operator()(const crucible::core::typing::StringPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::PointerPayload>
{
    size_t operator()(const crucible::core::typing::PointerPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::EnumPayload>
{
    size_t operator()(const crucible::core::typing::EnumPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::ListPayload>
{
    size_t operator()(const crucible::core::typing::ListPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::SetPayload>
{
    size_t operator()(const crucible::core::typing::SetPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::MapPayload>
{
    size_t operator()(const crucible::core::typing::MapPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::StructPayload>
{
    size_t operator()(const crucible::core::typing::StructPayload&) const;
};

template<>
struct std::hash<crucible::core::typing::JokerPayload>
{
    size_t operator()(const crucible::core::typing::JokerPayload&) const;
};
