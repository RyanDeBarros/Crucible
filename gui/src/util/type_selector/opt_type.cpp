#include <crucible/gui/util/type_selector/opt_type.hpp>

namespace crucible::gui
{

OptType::OptType(core::typing::Simplex simplex)
    : simplex(simplex)
{
}

OptType::OptType(const core::typing::Type& type)
    : simplex({ type.simplex() })
{
    for (size_t i = 0; i < simplex->argcount(); ++i)
        arguments.emplace_back(type.underlying(i));
}

std::optional<core::typing::Type> OptType::toType() const
{
    if (!simplex)
        return std::nullopt;

    std::vector<core::typing::Type> args;

    for (const auto& arg : arguments)
    {
        if (auto type = arg.toType())
            args.push_back(std::move(*type));
        else
            return std::nullopt;
    }

    // TODO instead of make_*(), just do make(Kind) with overloads for different numbers of arguments - then throw error if arg count doesn't match.
    switch (simplex->kind)
    {
    case core::typing::Kind::Bool:
        return core::typing::Type::make_bool();

    case core::typing::Kind::Int:
        return core::typing::Type::make_int();

    case core::typing::Kind::Float:
        return core::typing::Type::make_float();

    case core::typing::Kind::String:
        return core::typing::Type::make_string();

    case core::typing::Kind::Pointer:
        return core::typing::Type::make_pointer(args[0]);

    case core::typing::Kind::Enum:
        return core::typing::Type::make_enum(simplex->user);

    case core::typing::Kind::List:
        return core::typing::Type::make_list(args[0]);

    case core::typing::Kind::Set:
        return core::typing::Type::make_set(args[0]);

    case core::typing::Kind::Map:
        return core::typing::Type::make_map(args[0], args[1]);

    case core::typing::Kind::Struct:
        return core::typing::Type::make_struct(simplex->user);

    case core::typing::Kind::Joker:
        return core::typing::Type::make_joker();

    default:
        return {};
    }
}

} // crucible::gui
