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

    return core::typing::Type::make_generic(*simplex, args.data(), args.size());
}

} // crucible::gui
