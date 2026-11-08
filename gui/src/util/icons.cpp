#include <crucible/gui/util/icons.hpp>

#include <QIcon>

#include <stdexcept>

namespace crucible::gui
{

static std::unordered_map<Icon, std::unique_ptr<QIcon>> cached_icons;

static QIcon loadIconImpl(Icon icon)
{
    switch (icon)
    {
    case Icon::ClearLog:
        return QIcon(QStringLiteral(":/icons/clear_log"));

    case Icon::Filter:
        return QIcon(QStringLiteral(":/icons/filter"));

    case Icon::WordWrap:
        return QIcon(QStringLiteral(":/icons/word_wrap"));

    default:
        throw std::runtime_error("Unsupported icon");
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
