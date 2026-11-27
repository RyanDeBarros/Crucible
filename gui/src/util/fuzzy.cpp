#include <crucible/gui/util/fuzzy.hpp>

#include <QString>

namespace crucible::gui
{

bool fuzzyContains(const QString& text, const QString& subtext)
{
    qsizetype idx = 0;
    qsizetype sub_idx = 0;

    while (idx < text.size() && sub_idx < subtext.size())
    {
        if (text[idx++].toLower() == subtext[sub_idx].toLower())
            ++sub_idx;
    }

    return sub_idx == subtext.size();
}

} // crucible::gui
