#include <crucible/gui/util/dialog_underlay.hpp>

#include <QDialog>

namespace crucible::gui
{

void attachUnderlay(QDialog& dialog, QWidget& dialog_parent)
{
    auto* bkg_overlay = new QWidget(&dialog_parent);
    bkg_overlay->setStyleSheet("background-color: rgba(0, 0, 0, 200);");
    bkg_overlay->setGeometry(dialog_parent.rect());
    bkg_overlay->raise();
    bkg_overlay->show();

    dialog.connect(&dialog, &QDialog::finished, bkg_overlay, &QWidget::deleteLater);
}

} // crucible::gui
