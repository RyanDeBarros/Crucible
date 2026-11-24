#pragma once

#include <crucible/core/typing/qualified_type.hpp>

#include <QString>

namespace crucible::gui
{

extern QString repr(const core::typing::QualifiedType& type);
extern bool reservedTypeName(const QString& name);
extern bool validCharactersInTypeName(const QString& name);

} // crucible::gui
