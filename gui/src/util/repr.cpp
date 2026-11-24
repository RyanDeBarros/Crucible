#include <crucible/gui/util/repr.hpp>

#include <crucible/core/typing/nominal_registries.hpp>

namespace crucible::gui
{

QString repr(const core::typing::QualifiedType& type)
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
        if (auto underlying = type.underlying(0))
            return repr(*underlying) + QStringLiteral("*");
        else
            throw std::runtime_error("Unsupported type");
    case crucible::core::typing::Kind::Enum:
        return QString::fromStdString(core::typing::gEnumRegistry.enum_title(
            core::typing::gEnumRegistry.lock(),
            type.simplex().user));
    case crucible::core::typing::Kind::List:
        if (auto underlying = type.underlying(0))
            return QStringLiteral("list[") + repr(*underlying) + QStringLiteral("]");
        else
            throw std::runtime_error("Unsupported type");
    case crucible::core::typing::Kind::Set:
        if (auto underlying = type.underlying(0))
            return QStringLiteral("set[") + repr(*underlying) + QStringLiteral("]");
        else
            throw std::runtime_error("Unsupported type");
    case crucible::core::typing::Kind::Map:
    {
        auto key = type.underlying(0);
        auto value = type.underlying(1);
        if (key && value)
            return QStringLiteral("map[") + repr(*key) + QStringLiteral(", ") + repr(*value) + QStringLiteral("]");
        else
            throw std::runtime_error("Unsupported type");
    }
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

// TODO validate these for enum and struct titles (user instances)

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

} // crucible::gui
