#include <crucible/gui/util/icons.hpp>

#include <crucible/gui/logging/logger_include.hpp>

#include <QIcon>

#include <stdexcept>

namespace crucible::gui
{

static std::unordered_map<Icon, std::unique_ptr<QIcon>> cached_icons;

std::ostream& operator<<(std::ostream& os, Icon icon)
{
    switch (icon)
    {
    case Icon::BackArrow:
        return os << "Back Arrow";

    case Icon::ClearLog:
        return os << "Clear Log";

    case Icon::Close:
        return os << "Close";

    case Icon::Delete:
        return os << "Delete";

    case Icon::DeleteAll:
        return os << "Delete All";

    case Icon::Edit:
        return os << "Edit";

    case Icon::EditEnum:
        return os << "Edit Enum";

    case Icon::EditStruct:
        return os << "Edit Struct";

    case Icon::Filter:
        return os << "Filter";

    case Icon::NewModule:
        return os << "New Module";

    case Icon::Plus:
        return os << "Plus";

    case Icon::WordWrap:
        return os << "Word Wrap";

    default:
        return os << "<unknown #" << static_cast<int>(icon) << ">";
    }
}

static QIcon loadIconImpl(Icon icon)
{
    switch (icon)
    {
    case Icon::BackArrow:
        return QIcon(QStringLiteral(":/icons/back_arrow"));

    case Icon::ClearLog:
        return QIcon(QStringLiteral(":/icons/clear_log"));

    case Icon::Close:
        return QIcon(QStringLiteral(":/icons/close"));

    case Icon::Delete:
        return QIcon(QStringLiteral(":/icons/delete"));

    case Icon::DeleteAll:
        return QIcon(QStringLiteral(":/icons/delete_all"));

    case Icon::Edit:
        return QIcon(QStringLiteral(":/icons/edit"));

    case Icon::EditEnum:
        return QIcon(QStringLiteral(":/icons/edit_enum"));

    case Icon::EditStruct:
        return QIcon(QStringLiteral(":/icons/edit_struct"));

    case Icon::Filter:
        return QIcon(QStringLiteral(":/icons/filter"));

    case Icon::NewModule:
        return QIcon(QStringLiteral(":/icons/new_module"));

    case Icon::Plus:
        return QIcon(QStringLiteral(":/icons/plus"));

    case Icon::WordWrap:
        return QIcon(QStringLiteral(":/icons/word_wrap"));

    default:
    {
        LOG.warning(kApp) << "Unsupported icon \"" << icon << "\"";
        return {};
    }
    }
}

const QIcon& loadIcon(Icon icon)
{
    auto it = cached_icons.find(icon);
    if (it != cached_icons.end())
        return *it->second;
    else
    {
        return *cached_icons.emplace(
            icon,
            std::make_unique<QIcon>(loadIconImpl(icon))).first->second;
    }
}

} // crucible::gui
