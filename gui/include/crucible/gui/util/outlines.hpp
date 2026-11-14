#pragma once

class QLineEdit;
class QString;

namespace crucible::gui
{

extern void syncErrorOutline(QLineEdit& edit, bool error, const QString& tooltip);

} // crucible::gui
