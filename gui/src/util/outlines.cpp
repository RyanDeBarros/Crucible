#include <crucible/gui/util/outlines.hpp>

#include <QWidget>
#include <QString>

namespace crucible::gui
{

void syncErrorOutline(QWidget& widget, bool error, const QString& tooltip)
{
    if (error)
    {
        widget.setStyleSheet(QStringLiteral("QLineEdit { border: 1px solid #E53935; }"));
        widget.setToolTip(tooltip);
    }
    else
    {
        widget.setStyleSheet({});
        widget.setToolTip({});
    }
}

} // crucible::gui
