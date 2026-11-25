#pragma once

class QWidget;
class QString;

namespace crucible::gui::alerts
{

extern void cannotApplyChanges(QWidget& parent, const QString& description);

} // crucible::gui::alerts
