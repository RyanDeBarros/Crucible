#pragma once

#include <crucible/core/modules/module_id.hpp>

#include <crucible/core/typing/reflector.hpp>
#include <crucible/core/typing/qualified_type.hpp>

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
    std::vector<reflector::Box> value; // must contain payloads

    bool operator==(const ListPayload&) const = default;
};

struct SetPayload
{
    std::unordered_set<reflector::Box> value; // must contain payloads

    bool operator==(const SetPayload&) const = default;
};

struct MapPayload
{
    std::unordered_map<reflector::Box, reflector::Box> value; // must contain payloads

    bool operator==(const MapPayload&) const = default;
};

struct StructPayload
{
    std::vector<reflector::Box> value; // must contain payloads

    bool operator==(const StructPayload&) const = default;
};

struct JokerPayload
{
    reflector::Box value; // must contain a payload

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
    reflector::Box payload_;
    StorageState state_;

    Storage(QualifiedType type, reflector::Box payload, StorageState state);

public:
    const QualifiedType& type() const;
    StorageState state() const;

    static Storage make_uninitialized(QualifiedType type);
    static Storage make_initialized(QualifiedType type, reflector::Box payload);
    static Storage make_null(QualifiedType type);

    static bool is_valid_payload(const QualifiedType& type, const reflector::Box& payload);

    const reflector::Box* get(const QualifiedType& type) const;
    reflector::Box* get(const QualifiedType& type);
    bool set(Storage other);
};

} // crucible::core::typing
