#pragma once

#include <crucible/gui/util/modal_dialog.hpp>

class QLineEdit;

namespace crucible::gui
{

class NewEnumDialog
    : public ModalDialog
{
    Q_OBJECT;

private:
    QLineEdit* title_edit_ = nullptr;

public:
    explicit NewEnumDialog(QWidget* parent = nullptr);

private:
    QHBoxLayout* initTitle();
    QDialogButtonBox* initButtonBox();

    void createEnum();
};

} // crucible::gui
