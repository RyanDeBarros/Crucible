#pragma once

#include <crucible/core/modules/module.hpp>

#include <crucible/core/utility/registry.hpp>

namespace crucible::core::modules
{

class ModuleRegistry
    : private Registry<Module, ModuleId>
{
    using Super = Registry<Module, ModuleId>;

public:
    using Super::add;
    using Super::remove;
    using Super::exists;
    using Super::get;

    const typing::QualifiedType& module_type(ModuleId id) const;
};

extern ModuleRegistry gModuleRegistry;

} // crucible::core::modules
