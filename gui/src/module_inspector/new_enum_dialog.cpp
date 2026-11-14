#include <crucible/gui/module_inspector/new_enum_dialog.hpp>

#include <crucible/gui/util/dialog_underlay.hpp>

#include <crucible/gui/logging/logger_include.hpp>

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace crucible::gui
{

NewEnumDialog::NewEnumDialog(QWidget* parent)
    : ModalDialog(tr("New Enum"), QSize(720, 405), parent)
{
    auto* layout = new QVBoxLayout(this);

    layout->addLayout(initTitle());
    layout->addWidget(initButtonBox());
}

QHBoxLayout* NewEnumDialog::initTitle()
{
    auto* title_layout = new QHBoxLayout(this);

    auto* title_label = new QLabel(this);
    title_label->setText(tr("Enum name"));
    title_layout->addWidget(title_label);

    title_edit_ = new QLineEdit(this);
    title_edit_->setText(tr("New enum")); // TODO use (#) suffix if default already exists in gEnumRegistry

    title_layout->addWidget(title_edit_);

    return title_layout;
}

QDialogButtonBox* NewEnumDialog::initButtonBox()
{
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttons, &QDialogButtonBox::accepted, this, &NewEnumDialog::createEnum);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    return buttons;
}

void NewEnumDialog::createEnum()
{
    // TODO validate Enum title doesn't exist already - add lut to gEnumRegistry for titles
    LOG.info(kModules) << "Create new enum: " << title_edit_->text().toStdString(); // TODO
    accept();
}

} // crucible::gui
