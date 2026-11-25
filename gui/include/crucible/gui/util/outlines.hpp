#pragma once

class QWidget;
class QString;

namespace crucible::gui
{

extern void syncErrorOutline(QWidget& widget, bool error, const QString& tooltip);

} // crucible::gui
