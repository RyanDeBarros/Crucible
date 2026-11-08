#pragma once

#include <crucible/core/modules/module_id.hpp>

#include <crucible/core/typing/box.hpp>
#include <crucible/core/typing/qualified_type.hpp>

#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace crucible::core::typing
{

struct BoolPayload
{
    bool value;
};

struct IntPayload
{
    std::int64_t value;
};

struct FloatPayload
{
    double value;
};

struct StringPayload
{
    std::string value;
};

struct PointerPayload
{
    modules::ModuleId value;
};

struct EnumPayload
{
    std::uint64_t value;
};

template<typename T> // T must be payload type
struct ListPayload
{
    std::vector<T> value;
};

template<typename T> // T must be payload type
struct SetPayload
{
    std::unordered_set<T> value;
};

template<typename K, typename V> // K and V must be payload types
struct MapPayload
{
    std::unordered_map<K, V> value;
};

struct StructPayload
{
    std::vector<Box> value; // must contain payloads
};

struct JokerPayload
{
    Box value; // must contain a payload
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
    Box payload_;
    StorageState state_;

    Storage(QualifiedType type, Box payload, StorageState state);

public:
    const QualifiedType& type() const;
    StorageState state() const;

    static Storage make_uninitialized(QualifiedType type);
    static Storage make_initialized(QualifiedType type, Box payload);
    static Storage make_null(QualifiedType type);

    const Box* get(const QualifiedType& type) const;
    Box* get(const QualifiedType& type);
    bool set(Storage other);
};

} // crucible::core::typing
