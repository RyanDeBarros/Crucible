#pragma once

#include <crucible/gui/util/modal_dialog.hpp>

namespace crucible::gui
{

class NewModuleDialog
    : public ModalDialog
{
    Q_OBJECT;

public:
    explicit NewModuleDialog(QWidget* parent = nullptr);
};

} // crucible::gui
