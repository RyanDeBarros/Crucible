#include <crucible/core/typing/struct_registry.hpp>

#include <stdexcept>
#include <unordered_set>

namespace crucible::core::typing
{

std::vector<StructRegistry::Slot> StructRegistry::slots_{ 1 }; // reserve 0 for invalid kind
KindInstance StructRegistry::free_head_ = 0;
std::mutex StructRegistry::mutex_;

bool StructTemplate::valid() const
{
    std::unordered_set<std::string> names;
    for (const auto& [name, type] : fields)
    {
        if (!names.insert(name).second)
            return false;
    }

    return true;
}

void StructTemplate::assert_valid() const
{
    if (!valid())
        throw std::logic_error("Struct template has duplicate field names");
}

KindInstance StructRegistry::new_struct(StructTemplate tmpl)
{
    tmpl.assert_valid();

    std::unique_lock lock(mutex_);

    KindInstance kind;

    if (free_head_ != 0)
    {
        kind = free_head_;
        free_head_ = slots_[kind].next_free;
    }
    else
    {
        kind = static_cast<KindInstance>(slots_.size());
        slots_.emplace_back();
    }

    slots_[kind].definition.emplace(std::move(tmpl));
    slots_[kind].next_free = 0;

    return kind;
}

bool StructRegistry::delete_struct(KindInstance kind)
{
    std::unique_lock lock(mutex_);

    if (auto* slot = try_get_slot_unsafe(kind))
    {
        slot->definition.reset();
        slot->next_free = free_head_;
        free_head_ = kind;

        return true;
    }
    else
        return false;
}

bool StructRegistry::struct_exists(KindInstance kind)
{
    std::unique_lock lock(mutex_);
    return try_get_slot_unsafe(kind) != nullptr;
}

std::string StructRegistry::struct_title(KindInstance kind)
{
    std::unique_lock lock(mutex_);
    return get_slot_unsafe(kind).definition->title;
}

std::string StructRegistry::struct_field_name(KindInstance kind, std::uint64_t index)
{
    std::unique_lock lock(mutex_);

    const auto& fields = get_slot_unsafe(kind).definition->fields;
    if (index >= fields.size())
        throw std::out_of_range("Struct field index out of range");

    return fields[index].first;
}

QualifiedType StructRegistry::struct_field_type(KindInstance kind, std::uint64_t index)
{
    std::unique_lock lock(mutex_);

    const auto& fields = get_slot_unsafe(kind).definition->fields;
    if (index >= fields.size())
        throw std::out_of_range("Struct field index out of range");

    return fields[index].second;
}

QualifiedType StructRegistry::struct_field_type(KindInstance kind, const std::string_view name)
{
    std::unique_lock lock(mutex_);

    const auto& fields = get_slot_unsafe(kind).definition->fields;
    for (const auto& [fname, ftype] : fields)
    {
        if (fname == name)
            return ftype;
    }

    throw std::logic_error("Struct field does not exist");
}

StructRegistry::Slot* StructRegistry::try_get_slot_unsafe(KindInstance kind)
{
    if (kind == 0 || kind >= slots_.size() || !slots_[kind].definition)
        return nullptr;
    else
        return &slots_[kind];
}

StructRegistry::Slot& StructRegistry::get_slot_unsafe(KindInstance kind)
{
    if (auto* slot = try_get_slot_unsafe(kind))
        return *slot;
    else
        throw std::out_of_range("Struct does not exist");
}

} // namespace crucible::core::typing
