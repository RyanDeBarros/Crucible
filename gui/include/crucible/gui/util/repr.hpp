#pragma once

#include <crucible/core/typing/user_registries.hpp>

#include <crucible/core/typing/type.hpp>

#include <QString>

namespace crucible::gui
{

extern QString repr(const core::typing::UserRegistryLock& lock, const core::typing::Type& type);
extern QString repr(const core::typing::UserRegistryLock& lock, const core::typing::Simplex simplex);
extern bool reservedTypeName(const QString& name);
extern bool validCharactersInTypeName(const QString& name);

} // crucible::gui
