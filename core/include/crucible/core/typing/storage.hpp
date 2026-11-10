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

// TODO use templates again
struct ListPayload
{
    std::vector<reflector::box> value; // must contain payloads

    bool operator==(const ListPayload&) const = default;
};

struct SetPayload
{
    std::unordered_set<reflector::box> value; // must contain payloads

    bool operator==(const SetPayload&) const = default;
};

struct MapPayload
{
    std::unordered_map<reflector::box, reflector::box> value; // must contain payloads

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
