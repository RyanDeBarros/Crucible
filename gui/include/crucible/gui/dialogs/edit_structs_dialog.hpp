#pragma once

#include <crucible/gui/util/modal_dialog.hpp>

namespace crucible::gui
{

class EditStructsDialog
    : public ModalDialog
{
    Q_OBJECT;

public:
    explicit EditStructsDialog(QWidget* parent = nullptr);
};

} // crucible::gui
