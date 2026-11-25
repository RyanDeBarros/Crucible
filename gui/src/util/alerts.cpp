#include <crucible/gui/util/alerts.hpp>

#include <QMessageBox>

namespace crucible::gui::alerts
{

void cannotApplyChanges(QWidget& parent, const QString& description)
{
    QMessageBox::critical(
        &parent,
        parent.tr("Cannot apply changes"),
        description,
        QMessageBox::Ok,
        QMessageBox::Ok);
}

} // crucible::gui::alerts
