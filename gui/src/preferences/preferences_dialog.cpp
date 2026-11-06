#include <crucible/gui/preferences/preferences_dialog.hpp>

#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QWidget>

namespace crucible::gui
{

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Preferences"));
    setModal(true);
    resize(1440, 810);

    createLayout();
}

void PreferencesDialog::createLayout()
{
    auto* layout = new QVBoxLayout(this);

    tabs_ = new QTabWidget(this);
    layout->addWidget(tabs_);

    buttons_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons_);

    connect(buttons_, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons_, &QDialogButtonBox::rejected, this, &QDialog::reject);

    tabs_->addTab(new QWidget(tabs_), tr("General"));
    tabs_->addTab(new QWidget(tabs_), tr("Logging"));
}

} // crucible::gui
