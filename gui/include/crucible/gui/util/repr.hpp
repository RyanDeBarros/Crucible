#pragma once

#include <crucible/core/typing/type.hpp>

#include <QString>

namespace crucible::gui
{

extern QString repr(const core::typing::Type& type);
extern bool reservedTypeName(const QString& name);
extern bool validCharactersInTypeName(const QString& name);

extern const QString kUnselectedTypeName;

} // crucible::gui
