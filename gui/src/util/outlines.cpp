#include <crucible/gui/util/outlines.hpp>

#include <QLineEdit>
#include <QString>

namespace crucible::gui
{

void syncErrorOutline(QLineEdit& edit, bool error, const QString& tooltip)
{
    if (error)
    {
        edit.setStyleSheet(QStringLiteral("QLineEdit { border: 1px solid #E53935; }"));
        edit.setToolTip(tooltip);
    }
    else
    {
        edit.setStyleSheet({});
        edit.setToolTip({});
    }
}

} // crucible::gui
