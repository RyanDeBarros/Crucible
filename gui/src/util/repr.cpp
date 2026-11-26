#include <crucible/gui/util/repr.hpp>

#include <QTextStream>

namespace crucible::gui
{

QString repr(const core::typing::UserRegistryLock& lock, const core::typing::Type& type)
{
    const auto simplex = type.simplex();
    const size_t argcount = simplex.argcount();

    if (argcount == 0)
        return repr(lock, simplex);
    else
    {
        QTextStream name;
        name << repr(lock, simplex) << QStringLiteral("[");
        for (size_t i = 0; i < argcount; ++i)
        {
            name << repr(lock, type.underlying(i));
            if (i + 1 < argcount)
                name << QStringLiteral(", ");
        }
        name << repr(lock, simplex) << QStringLiteral("]");
        return name.readAll();
    }
}

static QString builtin_repr(const core::typing::Kind kind)
{
    switch (kind)
    {
    case core::typing::Kind::Bool:
        return QStringLiteral("bool");

    case core::typing::Kind::Int:
        return QStringLiteral("int");

    case core::typing::Kind::Float:
        return QStringLiteral("float");

    case core::typing::Kind::String:
        return QStringLiteral("string");

    case core::typing::Kind::Pointer:
        return QStringLiteral("pointer");

    case core::typing::Kind::List:
        return QStringLiteral("list");

    case core::typing::Kind::Set:
        return QStringLiteral("set");

    case core::typing::Kind::Map:
        return QStringLiteral("map");

    case core::typing::Kind::Joker:
        return QStringLiteral("joker");

    default:
        throw std::runtime_error("Unsupported or user type");
    }
}

extern QString repr(const core::typing::UserRegistryLock& lock, const core::typing::Simplex simplex)
{
    switch (simplex.kind)
    {
    case core::typing::Kind::Enum:
        return QString::fromStdString(core::typing::gEnumRegistry.enum_title(lock.enum_lock, simplex.user));

    case core::typing::Kind::Struct:
        return QString::fromStdString(core::typing::gStructRegistry.struct_title(lock.struct_lock, simplex.user));

    default:
        return builtin_repr(simplex.kind);
    }
}

bool reservedTypeName(const QString& name)
{
    return name == builtin_repr(core::typing::Kind::Bool)
        || name == builtin_repr(core::typing::Kind::Int)
        || name == builtin_repr(core::typing::Kind::Float)
        || name == builtin_repr(core::typing::Kind::String)
        || name == builtin_repr(core::typing::Kind::List)
        || name == builtin_repr(core::typing::Kind::Set)
        || name == builtin_repr(core::typing::Kind::Map)
        || name == builtin_repr(core::typing::Kind::Joker);
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
