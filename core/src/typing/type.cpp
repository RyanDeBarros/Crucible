#include <crucible/core/typing/type.hpp>

#include <crucible/core/typing/user_registries.hpp>

#include <stack>
#include <unordered_map>

namespace crucible::core::typing
{

size_t Simplex::argcount() const
{
    switch (kind)
    {
    case Kind::Pointer:
    case Kind::List:
    case Kind::Set:
        return 1;

    case Kind::Map:
        return 2;

    default:
        return 0;
    }
}

struct TypeNode
{
    Simplex simplex;
    std::array<detail::TypeNodeId, 2> underlying;

    bool operator==(const TypeNode& o) const = default;
};

struct TypeNodeHash
{
    size_t operator()(const TypeNode& n) const
    {
        size_t h = std::hash<Kind>{}(n.simplex.kind) ^ std::hash<UserInstance>{}(n.simplex.user);

        for (detail::TypeNodeId underlying : n.underlying)
            h ^= underlying + 0x9e3779b9 + (h << 6) + (h >> 2);

        return h;
    }
};

class TypeArena
{
    std::unordered_map<detail::TypeNodeId, TypeNode> map_;
    std::unordered_map<TypeNode, detail::TypeNodeId, TypeNodeHash> lut_;
    std::stack<detail::TypeNodeId> free_ids_;
    std::unordered_map<detail::TypeNodeId, size_t> users_;

    mutable std::mutex mutex_;

public:
    detail::TypeNodeId alloc(Simplex simplex)
    {
        std::lock_guard lock(mutex_);
        return alloc_unsafe({ .simplex = simplex });
    }

    detail::TypeNodeId alloc(Simplex simplex, detail::TypeNodeId underlying_0)
    {
        std::lock_guard lock(mutex_);
        return alloc_unsafe({ .simplex = simplex, .underlying = { underlying_0 } });
    }

    detail::TypeNodeId alloc(Simplex simplex, detail::TypeNodeId underlying_0, detail::TypeNodeId underlying_1)
    {
        std::lock_guard lock(mutex_);
        return alloc_unsafe({ .simplex = simplex, .underlying = { underlying_0, underlying_1 } });
    }

    void increment(detail::TypeNodeId id)
    {
        std::lock_guard lock(mutex_);
        increment_unsafe(id);
    }

    void decrement(detail::TypeNodeId id)
    {
        std::lock_guard lock(mutex_);
        decrement_unsafe(id);
    }

    TypeNode get(detail::TypeNodeId id)
    {
        std::lock_guard lock(mutex_);
        return get_unsafe(id);
    }

private:
    detail::TypeNodeId alloc_unsafe(const TypeNode& node)
    {
        if (auto it = lut_.find(node); it != lut_.end())
        {
            detail::TypeNodeId id = it->second;
            ++users_[id];
            return id;
        }

        detail::TypeNodeId id;
        if (free_ids_.empty())
        {
            id = map_.size() + 1;
        }
        else
        {
            id = free_ids_.top();
            free_ids_.pop();
        }

        map_[id] = node;
        lut_.emplace(node, id);
        users_[id] = 1;

        for (detail::TypeNodeId underlying : node.underlying)
        {
            if (underlying)
                increment_unsafe(underlying);
        }

        return id;
    }

    void increment_unsafe(detail::TypeNodeId id)
    {
        auto it = users_.find(id);
        if (it == users_.end())
            throw std::runtime_error("Cannot increment disowned node");

        ++it->second;
    }

    void decrement_unsafe(detail::TypeNodeId id)
    {
        auto it = map_.find(id);
        if (it == map_.end())
            return;

        if (--users_[id] > 0)
            return;

        TypeNode node = it->second;

        users_.erase(id);
        map_.erase(id);
        lut_.erase(node);
        free_ids_.push(id);

        for (detail::TypeNodeId underlying : node.underlying)
        {
            if (underlying)
                decrement_unsafe(underlying);
        }
    }

    TypeNode get_unsafe(detail::TypeNodeId id)
    {
        auto it = map_.find(id);
        if (it != map_.end())
            return it->second;
        else
            throw std::runtime_error("Type does not exist");
    }
};

static TypeArena& type_arena()
{
    static TypeArena* arena = new TypeArena();
    return *arena; // leak so that TypeArena is always available for Type::~Type() in static objects
}

Type::Type(detail::TypeNodeId id)
    : id_(id)
{
}

Type::Type(const Type& o)
    : id_(o.id_)
{
    if (id_)
        type_arena().increment(id_);
}

Type::Type(Type&& o) noexcept
    : id_(o.id_)
{
    o.id_ = 0;
}

Type::~Type()
{
    if (id_)
        type_arena().decrement(id_);
}

Type& Type::operator=(const Type& o)
{
    if (this != &o && id_ != o.id_)
    {
        if (id_)
            type_arena().decrement(id_);

        id_ = o.id_;
        if (id_)
            type_arena().increment(id_);
    }

    return *this;
}

Type& Type::operator=(Type&& o) noexcept
{
    if (this != &o)
    {
        if (id_)
            type_arena().decrement(id_);

        id_ = o.id_;
        o.id_ = 0;
    }

    return *this;
}

bool Type::operator==(const Type& o) const
{
    return id_ == o.id_;
}

bool Type::valid() const
{
    return id_ > 0;
}

Simplex Type::simplex() const
{
    return type_arena().get(id_).simplex;
}

Type Type::underlying(size_t idx) const
{
    TypeNode node = type_arena().get(id_);

    if (idx >= node.underlying.size() || !node.underlying[idx])
        throw std::out_of_range("Underlying type does not exist");

    detail::TypeNodeId u = node.underlying[idx];
    type_arena().increment(u);
    return Type(u);
}

size_t Type::hash() const
{
    return std::hash<detail::TypeNodeId>{}(id_);
}

Type Type::make_bool()
{
    return type_arena().alloc({ .kind = Kind::Bool });
}

Type Type::make_int()
{
    return type_arena().alloc({ .kind = Kind::Int });
}

Type Type::make_float()
{
    return type_arena().alloc({ .kind = Kind::Float });
}

Type Type::make_string()
{
    return type_arena().alloc({ .kind = Kind::String });
}

Type Type::make_pointer(const Type& underlying_type)
{
    return type_arena().alloc({ .kind = Kind::Pointer }, underlying_type.id_);
}

Type Type::make_enum(UserInstance user)
{
    return type_arena().alloc({ .kind = Kind::Enum, .user = user });
}

Type Type::make_list(const Type& elem_type)
{
    return type_arena().alloc({ .kind = Kind::List }, elem_type.id_);
}

Type Type::make_set(const Type& elem_type)
{
    return type_arena().alloc({ .kind = Kind::Set }, elem_type.id_);
}

Type Type::make_map(const Type& key_type, const Type& value_type)
{
    return type_arena().alloc({ .kind = Kind::Map }, key_type.id_, value_type.id_);
}

Type Type::make_struct(UserInstance user)
{
    return type_arena().alloc({ .kind = Kind::Struct, .user = user });
}

Type Type::make_joker()
{
    return type_arena().alloc({ .kind = Kind::Joker });
}

} // crucible::core::typing
