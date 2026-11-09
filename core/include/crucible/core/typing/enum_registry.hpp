#pragma once

#include <crucible/core/typing/qualified_type.hpp>

#include <mutex>
#include <optional>
#include <vector>

namespace crucible::core::typing
{

struct EnumTemplate
{
    std::string title;
    std::vector<std::string> names;
};

class EnumRegistry
{
    struct Slot
    {
        std::optional<EnumTemplate> definition;
        KindInstance next_free = 0;
    };

    static std::vector<Slot> slots_;
    static KindInstance free_head_;
    static std::mutex mutex_;

public:
    static KindInstance new_enum(EnumTemplate tmpl);
    static bool delete_enum(KindInstance kind);
    static bool enum_exists(KindInstance kind);

    static std::string enum_title(KindInstance kind);
    static std::string enum_name(KindInstance kind, std::uint64_t index);
    static std::uint64_t enum_count(KindInstance kind);

private:
    static Slot* try_get_slot_unsafe(KindInstance kind);
    static Slot& get_slot_unsafe(KindInstance kind);
};

} // namespace crucible::core::typing
