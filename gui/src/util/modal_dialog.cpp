#include <crucible/gui/util/modal_dialog.hpp>

#include <crucible/gui/util/dialog_underlay.hpp>

#include <crucible/gui/main_window.hpp>

#include <QEvent>
#include <QKeyEvent>

namespace crucible::gui
{

ModalDialog::ModalDialog(const QString& title, QSize size, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setModal(true);
    resize(size.width(), size.height());
    installEventFilter(this);

    attachUnderlay(*this, MainWindow::instance());
}

bool ModalDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this && event->type() == QEvent::KeyPress)
    {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
            return true; // enter key should not close dialog
    }

    return QDialog::eventFilter(watched, event);
}

} // crucible::gui
