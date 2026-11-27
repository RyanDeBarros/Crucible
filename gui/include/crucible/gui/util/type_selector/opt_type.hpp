#pragma once

#include <crucible/core/typing/type.hpp>

#include <optional>
#include <vector>

namespace crucible::gui
{

struct OptType
{
    std::optional<core::typing::Simplex> simplex = std::nullopt;
    std::vector<OptType> arguments;

    OptType() = default;
    OptType(core::typing::Simplex simplex);
    OptType(const core::typing::Type& type);

    std::optional<core::typing::Type> toType() const;
};

} // crucible::gui
