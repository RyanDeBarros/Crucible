#pragma once

#include <crucible/core/modules/metadata.hpp>
#include <crucible/core/modules/module_id.hpp>

#include <crucible/core/typing/storage.hpp>

namespace crucible::core::modules
{

struct Module
{
    typing::Storage storage;
    ModuleId address;
    Metadata metadata;
};

} // crucible::core::modules
