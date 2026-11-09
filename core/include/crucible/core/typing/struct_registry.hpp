#pragma once

#include <crucible/core/typing/qualified_type.hpp>

#include <map>
#include <mutex>
#include <optional>
#include <vector>

namespace crucible::core::typing
{

struct StructTemplate
{
    std::string title;
    std::vector<std::pair<std::string, QualifiedType>> fields;

    bool valid() const;
    void assert_valid() const;
};

class StructRegistry
{
    struct Slot
    {
        std::optional<StructTemplate> definition;
        KindInstance next_free = 0;
    };

    static std::vector<Slot> slots_;
    static KindInstance free_head_;
    static std::mutex mutex_;

public:
    static KindInstance new_struct(StructTemplate tmpl);
    static bool delete_struct(KindInstance kind);
    static bool struct_exists(KindInstance kind);

    static std::string struct_title(KindInstance kind);
    static std::string struct_field_name(KindInstance kind, std::uint64_t index);
    static QualifiedType struct_field_type(KindInstance kind, std::uint64_t index);
    static QualifiedType struct_field_type(KindInstance kind, const std::string_view name);

private:
    static Slot* try_get_slot_unsafe(KindInstance kind);
    static Slot& get_slot_unsafe(KindInstance kind);
};

} // namespace crucible::core::typing
