#include <crucible/gui/util/repr.hpp>

#include <crucible/core/typing/user_registries.hpp>

namespace crucible::gui
{

QString repr(const core::typing::Type& type)
{
    switch (type.simplex().kind)
    {
    case crucible::core::typing::Kind::Bool:
        return QStringLiteral("bool");
    case crucible::core::typing::Kind::Int:
        return QStringLiteral("int");
    case crucible::core::typing::Kind::Float:
        return QStringLiteral("float");
    case crucible::core::typing::Kind::String:
        return QStringLiteral("string");
    case crucible::core::typing::Kind::Pointer:
        return repr(type.underlying(0)) + QStringLiteral("*");
    case crucible::core::typing::Kind::Enum:
        return QString::fromStdString(core::typing::gEnumRegistry.enum_title(
            core::typing::gEnumRegistry.lock(),
            type.simplex().user));
    case crucible::core::typing::Kind::List:
        return QStringLiteral("list[") + repr(type.underlying(0)) + QStringLiteral("]");
    case crucible::core::typing::Kind::Set:
        return QStringLiteral("set[") + repr(type.underlying(0)) + QStringLiteral("]");
    case crucible::core::typing::Kind::Map:
        return QStringLiteral("map[") + repr(type.underlying(0)) + QStringLiteral(", ")
            + repr(type.underlying(1)) + QStringLiteral("]");
    case crucible::core::typing::Kind::Struct:
        return QString::fromStdString(core::typing::gStructRegistry.struct_title(
            core::typing::gStructRegistry.lock(),
            type.simplex().user));
    case crucible::core::typing::Kind::Joker:
        return QStringLiteral("joker");
    default:
        throw std::runtime_error("Unsupported type");
    }
}

bool reservedTypeName(const QString& name)
{
    return name == QStringLiteral("bool")
        || name == QStringLiteral("int")
        || name == QStringLiteral("float")
        || name == QStringLiteral("string")
        || name == QStringLiteral("list")
        || name == QStringLiteral("set")
        || name == QStringLiteral("map")
        || name == QStringLiteral("joker");
}

bool validCharactersInTypeName(const QString& name)
{
    for (QChar c : name)
    {
        if (c == QChar('[') ||
            c == QChar(']') ||
            c == QChar('*'))
        {
            return false;
        }
    }

    return true;
}

const QString kUnselectedTypeName = "-- select --";

} // crucible::gui
