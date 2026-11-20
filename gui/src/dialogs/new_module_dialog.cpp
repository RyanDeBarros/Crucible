#include <crucible/gui/dialogs/new_module_dialog.hpp>

namespace crucible::gui
{

NewModuleDialog::NewModuleDialog(QWidget* parent)
    : ModalDialog(tr("New Module"), QSize(720, 405), parent)
{
}

} // crucible::gui
